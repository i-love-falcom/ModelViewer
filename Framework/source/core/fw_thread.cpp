/**
 * @file fw_thread.cpp
 * @author Kamai Masayoshi
 */
#include "precompiled.h"
#include "core/fw_thread.h"

BEGIN_NAMESPACE_FW
BEGIN_NAMESPACE_NONAME
/**
 * @struct ThreadInfo
 */
class ThreadInfo {
public:
    bool        terminate;
    bool        raiseThread;
    bool        waitWorkerThread;
    bool        isWorkerThread;
    char        name[MaxThreadNameLen + 1];
    sint32_t    exitCode;

    std::mutex                  threadMtx;
    std::condition_variable     raiseThreadCV;
    std::condition_variable     waitThreadCV;

    void Init(const bool worker = false) {
        terminate = false;
        raiseThread = false;
        waitWorkerThread = false;
        isWorkerThread = worker;

        name[0] = '\0';
        exitCode = 0;
    }

    ThreadInfo() {
    }
};

static void SetThreadName(Thread * thread) {
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
    info.dwThreadID = static_cast<DWORD>(thread->GetThreadId());
    info.dwFlags = 0;

    __try {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        //! @todo 実装
    }
#endif
}

static unsigned __stdcall ThreadFunc(void * userArgs) {
    Thread * thread = reinterpret_cast<Thread *>(userArgs);
    ThreadInfo * threadInfo = thread->GetThreadInfo();

    SetThreadName(thread);

    if (threadInfo->isWorkerThread) {
        while (true) {
            {
                std::unique_lock<std::mutex> lock(threadInfo->threadMtx);

                threadInfo->raiseThreadCV.wait(lock, [&] { return threadInfo->raiseThread; });
                threadInfo->raiseThread = false;
                threadInfo->waitWorkerThread = false;
            }
            
            if (!threadInfo->terminate) {
                threadInfo->exitCode = thread->Invoke(thread->GetDesc().userArgs);
            }

            // ワーカースレッドの終了を通知
            {
                std::lock_guard<std::mutex> lock(threadInfo->threadMtx);
                threadInfo->waitWorkerThread = true;
            }
            threadInfo->waitThreadCV.notify_all();

            if (threadInfo->terminate) {
                break;
            }
        }
    } else {
        threadInfo->exitCode = thread->Invoke(thread->GetDesc().userArgs);
    }

#if !defined(FW_PLATFORM_WIN32)
    // このスレッド終了時に終了待ちスレッドを全て起こす
    {
        std::unique_lock<std::mutex> lock(threadInfo->threadMtx);
        std::notify_all_at_thread_exit(threadInfo->waitThreadCV, std::move(lock));
    }
#endif

    return static_cast<unsigned>(threadInfo->exitCode);
}
END_NAMESPACE_NONAME

Thread::Thread()
: threadId(0)
, threadHandle(0)
, threadInfo(nullptr) {
}

Thread::~Thread() {
}

void Thread::Shutdown() {
    TerminateThread();

    if (threadInfo != nullptr) {
        ThreadInfo * ptr = threadInfo;
        threadInfo = nullptr;

        FwDelete(ptr);
    }
}

void Thread::Start(const ThreadDesc * threadDesc) {
    if (threadDesc == nullptr) {
        return;
    }
    desc = threadDesc[0];

    StartThread(false);
}

void Thread::StartWorker(const ThreadDesc * threadDesc) {
    if (threadDesc == nullptr) {
        return;
    }
    desc = threadDesc[0];

    StartThread(true);
}

void Thread::StartThread(const bool workerThread) {
    if (threadInfo == nullptr) {
        threadInfo = FwNew<ThreadInfo>();
    }
    threadInfo->Init(workerThread);

#if defined(FW_PLATFORM_WIN32)
    #if defined(FW_UNICODE)
    size_t dstResult = 0;
    tstring::WCharToChar(&dstResult, threadInfo->name, ARRAY_SIZEOF(threadInfo->name), desc.name, ARRAY_SIZEOF(desc.name));
    #endif

    threadHandle = _beginthreadex(
        nullptr,
        static_cast<unsigned>(desc.stackSize),
        ThreadFunc,
        this,
        CREATE_SUSPENDED,
        reinterpret_cast<unsigned *>(&threadId));

    ::SetThreadAffinityMask(reinterpret_cast<HANDLE>(threadHandle), desc.affinity);
    ::SetThreadPriority(reinterpret_cast<HANDLE>(threadHandle), desc.priority);
    ::ResumeThread(reinterpret_cast<HANDLE>(threadHandle));
#else
    threadInstance.swap(std::thread(ThreadFunc, this));

    threadId = threadInstance.get_id();
    threadHandle = threadInstance.native_handle();
#endif
}

void Thread::TerminateThread() {
    if (threadInfo != nullptr) {
        threadInfo->terminate = true;
        RaiseWorkerThread();
    }
}

void Thread::RaiseWorkerThread() {
    if (threadInfo != nullptr && threadInfo->isWorkerThread) {
        {
            std::lock_guard<std::mutex> lock(threadInfo->threadMtx);
            threadInfo->raiseThread = true;
        }
        threadInfo->raiseThreadCV.notify_one();
    }
}

sint32_t Thread::WaitThread(const uint32_t millisecond) {
    if (threadHandle != 0) {
        std::unique_lock<std::mutex> lock(threadInfo->threadMtx);

        if (threadInfo->isWorkerThread) {
            bool result = threadInfo->waitThreadCV.wait_for(lock, std::chrono::milliseconds(millisecond), [&] {return threadInfo->waitWorkerThread;});
            return result ? FW_OK : WAIT_TIMEOUT;
        } else {
#if defined(FW_PLATFORM_WIN32)
            DWORD result = WaitForSingleObject((HANDLE)threadHandle, millisecond);

            return result == WAIT_OBJECT_0 ? FW_OK : (result == WAIT_TIMEOUT ? ERR_TIMEOUT : ERR_FAILED);
#else
            std::cv_status stat = threadInfo->waitThreadCV.wait_for(lock, std::chrono::milliseconds(millisecond));
            return stat == std::cv_status::timeout ? WAIT_TIMEOUT : FW_OK;
#endif
        }
    }
    return FW_OK;
}

uint32_t Thread::GetExitCode() {
    if (threadInfo != nullptr) {
        return threadInfo->exitCode;
    }
    return 0xffffffffu;
}

//-------------------------------------------
// static method
//-------------------------------------------
threadId_t Thread::GetCurrentThreadId() {
#if defined(FW_PLATFORM_WIN32)
    return static_cast<threadId_t>(::GetCurrentThreadId());
#else
    return std::this_thread::get_id();
#endif
}

void Thread::Exit(uint32_t exitCode) {
#if defined(FW_PLATFORM_WIN32)
    _endthreadex(exitCode);
#endif
}

void Thread::Sleep(uint32_t millisecond) {
#if defined(FW_PLATFORM_WIN32)
    SleepEx(static_cast<DWORD>(millisecond), FALSE);
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(millisecond));
#endif
}

void Thread::YieldThread() {
#if defined(FW_PLATFORM_WIN32)
    SwitchToThread();
#else
    std::this_thread::yield();
#endif
}

void Thread::WaitAny(const Thread * threadArray[], const uint32_t count, sint32_t * results, const uint32_t millisecond) {
#if defined(FW_PLATFORM_WIN32)
    HANDLE handleArray[MAXIMUM_WAIT_OBJECTS];
    for (uint32_t i = 0; i < count; ++i) {
        handleArray[i] = reinterpret_cast<HANDLE>(threadArray[i]->GetThreadHandle());
    }
    DWORD r = WaitForMultipleObjects(count, handleArray, FALSE, millisecond);

    if (results != nullptr) {
        if (r == WAIT_FAILED) {
            results[0] = ERR_FAILED;
        } else if (WAIT_OBJECT_0 <= r && r < WAIT_OBJECT_0 + count) {
            results[0] = FW_OK;
        } else if (WAIT_ABANDONED_0 <= r && r < WAIT_ABANDONED_0 + count) {
            results[0] = FW_OK;
        } else if (r == WAIT_TIMEOUT) {
            results[0] = ERR_TIMEOUT;
        }
        results[0] = ERR_UNKNOWN;
    }
#else
    #error Unsupported
#endif
}

void Thread::WaitAll(const Thread * threadArray[], const uint32_t count, sint32_t * results, const uint32_t millisecond) {
#if defined(FW_PLATFORM_WIN32)
    HANDLE handleArray[MAXIMUM_WAIT_OBJECTS];
    for (uint32_t i = 0; i < count; ++i) {
        handleArray[i] = reinterpret_cast<HANDLE>(threadArray[i]->GetThreadHandle());
    }
    DWORD r = WaitForMultipleObjects(count, handleArray, TRUE, millisecond);

    if (results != nullptr) {
        if (r == WAIT_FAILED) {
            results[0] = ERR_FAILED;
        } else if (WAIT_OBJECT_0 <= r && r < WAIT_OBJECT_0 + count) {
            results[0] = FW_OK;
        } else if (WAIT_ABANDONED_0 <= r && r < WAIT_ABANDONED_0 + count) {
            results[0] = FW_OK;
        } else if (r == WAIT_TIMEOUT) {
            results[0] = ERR_TIMEOUT;
        }
        results[0] = ERR_UNKNOWN;
    }
#else
    #error Unsupported
#endif
}

END_NAMESPACE_FW