/**
 * @file fw_thread.cpp
 * @author Kamai Masayoshi
 */
#include "precompiled.h"
#include "core/fw_thread.h"

BEGIN_NAMESPACE_FW
/**
 * @class ThreadInfo
 */
class FwThreadInfo {
public:
    volatile bool   terminate;
    bool            raiseThread;
    bool            waitWorkerThread;
    bool            isWorkerThread;
    char            name[FwMaxThreadNameLen + 1];
    sint32_t        exitCode;

    std::mutex                  threadMtx;
    std::condition_variable     raiseThreadCV;
    std::condition_variable     waitThreadCV;
#if FW_THREAD == FW_THREAD_STL
    std::thread                 thread;
#endif

    void Init(const bool worker = false) {
        terminate = false;
        raiseThread = false;
        waitWorkerThread = false;
        isWorkerThread = worker;

        name[0] = '\0';
        exitCode = 0;
    }

    FwThreadInfo()
    : threadMtx()
    , raiseThreadCV()
    , waitThreadCV()
#if FW_THREAD == FW_THREAD_STL
    , thread()
#endif
    {
    }
};

BEGIN_NAMESPACE_NONAME
static void SetThreadName(FwThread * thread) {
#if defined(FW_PLATFORM_WIN32)
    const uint32_t MS_VC_EXCEPTION = 0x406d1388;

    #pragma pack(push,8)
    typedef struct tagTHREADNAME_INFO {
        DWORD dwType;        // Must be 0x1000.
        LPCSTR szName;       // Pointer to name (in user addr space).
        DWORD dwThreadID;    // Thread ID (-1=caller thread).
        DWORD dwFlags;       // Reserved for future use, must be zero.
    } THREADNAME_INFO;
    #pragma pack(pop)

    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = thread->GetThreadInfo()->name;
    info.dwThreadID = ::GetCurrentThreadId();
    info.dwFlags = 0;

    __try {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        //! @todo 実装
    }
#else
    //! @todo プラットフォーム対応
#endif
}

static unsigned __stdcall ThreadEntryFunction(void * userArgs) {
    FwThread * thread = reinterpret_cast<FwThread *>(userArgs);
    FwThreadInfo * threadInfo = thread->GetThreadInfo();

    SetThreadName(thread);

    // 起動後一旦ここで止める
    {
        std::unique_lock<std::mutex> lock(threadInfo->threadMtx);

        threadInfo->raiseThreadCV.wait(lock, [&] { return threadInfo->raiseThread; });
        threadInfo->raiseThread = false;
        threadInfo->waitWorkerThread = false;
    }

    // 初期化処理
    thread->InitializeThreadFunc(thread->GetDesc().userArgs);

    if (threadInfo->isWorkerThread) {
        while (!threadInfo->terminate) {
            threadInfo->exitCode = thread->ThreadFunc(thread->GetDesc().userArgs);

            // ワーカースレッドの終了を通知
            {
                std::lock_guard<std::mutex> lock(threadInfo->threadMtx);
                threadInfo->waitWorkerThread = true;
            }
            threadInfo->waitThreadCV.notify_all();

            if (!threadInfo->terminate) {
                std::unique_lock<std::mutex> lock(threadInfo->threadMtx);

                threadInfo->raiseThreadCV.wait(lock, [&] { return threadInfo->raiseThread; });
                threadInfo->raiseThread = false;
                threadInfo->waitWorkerThread = false;
            }
        }
    } else {
        threadInfo->exitCode = thread->ThreadFunc(thread->GetDesc().userArgs);
    }

    // 終了処理
    thread->ShutdownThreadFunc(threadInfo->exitCode, thread->GetDesc().userArgs);

    return 0;
}
END_NAMESPACE_NONAME

FwThread::FwThread()
: threadInfo(nullptr)
#if FW_THREAD == FW_THREAD_WIN32
, threadId(0)
, threadHandle(0)
#endif
{
}

FwThread::~FwThread() {
    Shutdown();
}

void FwThread::Shutdown() {
    TerminateThread();

    if (threadInfo != nullptr) {
        FwThreadInfo * ptr = threadInfo;
        threadInfo = nullptr;

        FwDelete(ptr);
    }
}

void FwThread::Start(const FwThreadDesc * threadDesc) {
    if (threadDesc == nullptr) {
        return;
    }
    desc = threadDesc[0];

    StartThread(false);
}

void FwThread::StartWorker(const FwThreadDesc * threadDesc) {
    if (threadDesc == nullptr) {
        return;
    }
    desc = threadDesc[0];

    StartThread(true);
}

void FwThread::StartThread(const bool workerThread) {
    if (threadInfo == nullptr) {
        threadInfo = FwNew<FwThreadInfo>();
    }
    threadInfo->Init(workerThread);

#if defined(FW_UNICODE)
    size_t dstResult = 0;
    tstring::WCharToChar(&dstResult, threadInfo->name, ARRAY_SIZEOF(threadInfo->name), desc.name, ARRAY_SIZEOF(desc.name));
#endif

#if FW_THREAD == FW_THREAD_WIN32
    threadHandle = _beginthreadex(
        nullptr,
        static_cast<unsigned>(desc.stackSize),
        ThreadEntryFunction,
        this,
        0,
        reinterpret_cast<unsigned *>(&threadId));
#else
    threadInfo->thread = std::move(std::thread(ThreadEntryFunction, this));
#endif

#if defined(FW_PLATFORM_WIN32)
    FwThreadHandle hThread = GetThreadHandle();
    ::SetThreadAffinityMask(reinterpret_cast<HANDLE>(hThread), desc.affinity);
    ::SetThreadPriority(reinterpret_cast<HANDLE>(hThread), desc.priority);
#else
    //! @todo プラットフォーム対応
#endif

    // 初期化処理前で止まっているスレッドを起こす
    {
        std::lock_guard<std::mutex> lock(threadInfo->threadMtx);
        threadInfo->raiseThread = true;
    }
    threadInfo->raiseThreadCV.notify_one();
}

void FwThread::TerminateThread() {
    if (threadInfo != nullptr) {
        threadInfo->terminate = true;
        RestartThread();
    }
}

void FwThread::RestartThread() {
    if (threadInfo != nullptr && threadInfo->isWorkerThread) {
        {
            std::lock_guard<std::mutex> lock(threadInfo->threadMtx);
            threadInfo->raiseThread = true;
        }
        threadInfo->raiseThreadCV.notify_one();
    }
}

sint32_t FwThread::WaitThread(const uint32_t millisecond) {
    if (threadInfo != nullptr) {
        std::unique_lock<std::mutex> lock(threadInfo->threadMtx);

        if (threadInfo->isWorkerThread) {
            bool result = threadInfo->waitThreadCV.wait_for(lock, std::chrono::milliseconds(millisecond), [&] {return threadInfo->waitWorkerThread;});
            return result ? FW_OK : WAIT_TIMEOUT;
        } else {
#if FW_THREAD == FW_THREAD_WIN32
            DWORD result = WaitForSingleObject((HANDLE)threadHandle, millisecond);
            return result == WAIT_OBJECT_0 ? FW_OK : (result == WAIT_TIMEOUT ? ERR_TIMEOUT : ERR_FAILED);
#else
            threadInfo->thread.join();
            return FW_OK;
#endif
        }
    }
    return FW_OK;
}

uint32_t FwThread::GetExitCode() {
    if (threadInfo != nullptr) {
        return threadInfo->exitCode;
    }
    return 0xffffffffu;
}

FwThreadId FwThread::GetThreadId() const {
#if FW_THREAD == FW_THREAD_WIN32
    return threadId;
#elif FW_THREAD == FW_THREAD_STL
    return threadInfo != nullptr ? threadInfo->thread.get_id() : std::thread::id();
#endif
}

FwThreadHandle FwThread::GetThreadHandle() {
#if FW_THREAD == FW_THREAD_WIN32
    return threadHandle;
#elif FW_THREAD == FW_THREAD_STL
    return threadInfo != nullptr ? threadInfo->thread.native_handle() : nullptr;
#endif
}

//-------------------------------------------
// static method
//-------------------------------------------
FwThreadId FwThread::GetCurrentThreadId() {
#if FW_THREAD == FW_THREAD_WIN32
    return ::GetCurrentThreadId();
#elif FW_THREAD == FW_THREAD_STL
    return std::this_thread::get_id();
#endif
}

void FwThread::Sleep(uint32_t millisecond) {
    std::this_thread::sleep_for(std::chrono::milliseconds(millisecond));
}

void FwThread::YieldThread() {
    std::this_thread::yield();
}

END_NAMESPACE_FW