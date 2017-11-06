/**
 * @file core_file_system_win32.cpp
 * @author Kamai Masayoshi
 */
#include "stdafx.h"
#include "platform/win32/system/file/core_file_manager_win32.h"
#include "platform/win32/system/file/core_file_stream_win32.h"
#include "misc/core_time.h"
#include "system/memory/core_mem_allocator.h"

namespace core {
namespace {
static const sint32_t s_defaultMaxJobs = 32;
static const sint32_t s_defaultCoreId  = 0;
static const uint32_t s_workerThreadStackSize = 128 * 1024; // 128KiB
}

FileManagerWin32::FileManagerWin32()
: FileManager()
, allocator_(nullptr)
, workerThread_(nullptr)
, streamPool_(nullptr) {
}

FileManagerWin32::~FileManagerWin32() {
}

void FileManagerWin32::DoInit(FileManagerParms * parms, AllocatorBase * allocator) {
    assert(parms != nullptr);
    assert(allocator != nullptr);
    
    parms_ = *parms;
    allocator_ = allocator;

    // ストリームプールを確保
    streamPool_ = reinterpret_cast<FileStreamWin32 *>(MemAllocAllocator(allocator_,
        sizeof(FileStreamWin32) * parms_.streamPoolSize,
        MAKE_MEM_ATTRIBUTE(0, MEMTYPE_SYSTEM_DEFAULT, MEMTAG_FILE, false)));
    assert(streamPool_ != nullptr);

    // ストリームキューを初期化
    streamQueue_.assign(parms_.streamPoolSize);
    for (sint32_t i = 0; i < parms_.streamPoolSize; ++i) {
        streamQueue_.push_back(&streamPool_[i]);
    }
    
    // WorkerThread起動
    StartWorkerThread(parms->asyncThreadAffinity, parms->asyncThreadPriority_t);
}

void FileManagerWin32::DoShutdown() {
    assert(allocator_ != nullptr)

    AllocatorBase * alloc = allocator_;
    allocator_ = nullptr;
    
    StopWorkerThread();

    // ストリームプールを解放
    FileStreamWin32 * pool = streamPool_;
    streamPool_ = nullptr;
    MemFreeAllocator(alloc, pool);

    MemDeleteAllocator<FileManagerWin32>(alloc, this);
}

bool FileManagerWin32::DoIsFileExist(const str_t filePath) {
    HANDLE hFile = CreateFile(
        filePath,
        0,
        0,      // no share
        nullptr,   // security attributes
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }
    CloseHandle(hFile);

    return true;
}

CORE_RESULT FileManagerWin32::DoGetFileLength(const str_t filePath, uint64_t * length) {
    HANDLE hFile = CreateFile(
        filePath,
        0,
        0,      // no share
        nullptr,   // security attributes
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) {
        return ERR_INVALID;
    }

    LARGE_INTEGER len;
    if (!GetFileSizeEx(hFile, &len)) {
        return ERR_FILE_NOEXIST;
    }
    *length = static_cast<uint64_t>(len.QuadPart);

    CloseHandle(hFile);

    return R_OK;
}

FileStreamWin32 * FileManagerWin32::PopFileStream() {
    FileStreamWin32 * stream = nullptr;
    if (!streamQueue_.empty()) {
        SpinLock::ScopedLock cs(streamQueueLock_);
        stream = streamQueue_.front();
        streamQueue_.pop_front();
    }
    return stream;
}

void FileManagerWin32::PushFileStream(FileStreamWin32 * stream) {
    assert(stream != nullptr);
    SpinLock::ScopedLock cs(streamQueueLock_);
    streamQueue_.push_back(stream);
}

// ジョブパラメータをキューに追加
bool FileManagerWin32::PushJob(FileStreamJobNodeWin32 * job) {
    assert(job != nullptr);

    {
        SpinLock::ScopedLock cs(jobQueueLock_);
        jobQueue_.insert_sort(job);
    }
    return true;
}
    
// ジョブパラメータをキューから取り出す
FileStreamJobNodeWin32 * FileManagerWin32::PopJob() {
    FileStreamJobNodeWin32 * job = nullptr;
    {
        SpinLock::ScopedLock cs(jobQueueLock_);
        job = jobQueue_.front();
        jobQueue_.pop_front();
    }
    return job;
}

FileStream * FileManagerWin32::DoFileOpen(const str_t filePath, const sint32_t options, const uint32_t priority) {

    FileStreamWin32 * stream = PopFileStream();
    if (stream) {
        CORE_RESULT result = stream->Init(this, filePath, options, priority);
        if (result != R_OK) {
            PushFileStream(stream);
            return nullptr;
        }
    }
    return reinterpret_cast<FileStream *>(stream);
}

CORE_RESULT FileManagerWin32::DoFileDelete(const str_t filePath) {
    if (::DeleteFile(filePath) != 0) {
        return ERR_FAILED;
    }
    return R_OK;
}

sint32_t FileManagerWin32::AsyncReadWriteFunc(void * data) {
    AsyncReadWriteFuncParms * parms = reinterpret_cast<AsyncReadWriteFuncParms *>(data);
    assert(parms != nullptr);
    
    FileManagerWin32 * manager = parms->manager;
    assert(manager != nullptr);

    // @todo 可変にできるように
    const uint32_t spinTime     = 4096;
    const uint32_t suspendTime  = 4;
    
    uint32_t spinCount = 0;
    uint32_t suspendCount = 0;

    while (!parms->terminateThread) {
        // QueueからJobを取得
        FileStreamJobNodeWin32 * job = manager->PopJob();
        if (job == nullptr) {
            spinCount++;
            if (spinTime <= spinCount) {
                spinCount = 0;
                suspendCount++;
                if (suspendTime <= suspendCount) {
                    suspendCount = 0;
                    {
                        SpinLock::ScopedLock cs(parms->syncLock);
                        ::ResetEvent(parms->syncEvent);
                    }
                    ::WaitForSingleObject(parms->syncEvent, INFINITE);
                } else {
                    Thread::YieldThread();
                }
            }
            continue;
        }

        const uint64_t start = GetTickCount();

        // 実行
        job->DoWork();

        const uint64_t end = GetTickCount();
        
        // デバッグ
        if (0) {
            DebugPrintf(_T("%s %s proccessing time is %f usec\n"),
                job->name,
                job->opFlag == FILE_STREAM_JOB_FLAG_READ ? "read" : "write",
                CalcDiffMicroSecond(start, end));
        }
    }
    return 0;
}

void FileManagerWin32::StartWorkerThread(const uint32_t affinity, const uint32_t priority) {
    if (!workerThread_) {
        // パラメータ初期化
        workerThreadParms_.manager = this;
        workerThreadParms_.terminateThread = false;
        workerThreadParms_.syncEvent = ::CreateEvent(nullptr, TRUE, FALSE, _T("fm sync worker thread"));

        workerThread_ = MemNewAllocator<Thread>(allocator_, MAKE_MEM_ATTRIBUTE(0, MEMTYPE_SYSTEM_DEFAULT, MEMTAG_FILE, false));
        workerThread_->Init(_T("FileManagerWin32"), FileManagerWin32::AsyncReadWriteFunc, &workerThreadParms_, s_workerThreadStackSize);
        workerThread_->SetAffinity(affinity);
        workerThread_->SetPriority(priority);
        workerThread_->Resume();
    }
}

void FileManagerWin32::StopWorkerThread() {
    if (workerThread_) {
        // 終了通知
        workerThreadParms_.terminateThread = true;
        
        // WorkerThreadを起す
        RaiseWorkerThread();

        workerThread_->Wait();
        workerThread_->~Thread();

        // 同期用イベントを破棄
        ::CloseHandle(workerThreadParms_.syncEvent);

        MemFreeAllocator(allocator_, workerThread_);
    }
}

void FileManagerWin32::RaiseWorkerThread() {
    SpinLock::ScopedLock cs(workerThreadParms_.syncLock);
    ::SetEvent(workerThreadParms_.syncEvent);
}


//-----------------------------------------------
// CreateFileManagerWin32
//-----------------------------------------------
FileManagerWin32 * CreateFileManagerWin32(FileManagerParms * parms, AllocatorBase * allocator) {
    FileManagerWin32 * manager = MemNewAllocator<FileManagerWin32>(allocator, MAKE_MEM_ATTRIBUTE(0, MEMTYPE_SYSTEM_DEFAULT, MEMTAG_FILE, false));
    manager->Init(parms, allocator);

    return manager;
}

}   // namespace "core"
