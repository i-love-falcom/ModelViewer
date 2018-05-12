/**
 * @file core_file_stream_win32.cpp
 * @author Kamai Masayoshi
 */
#include "stdafx.h"
#include "platform/win32/system/file/core_file_stream_win32.h"
#include "platform/win32/system/file/core_file_manager_win32.h"

namespace core {
namespace {
static const size_t s_maxOpSize = 256 * 1024;
}   // namespace ""

FileStreamWin32::FileStreamWin32()
: FileStream()
, fileHandle_(NULL)
, manager_(nullptr)
, options_(0)
, priority_(FILE_PRIO_NORMAL)
, length_(0)
, seekOffset_(0) {
}

FileStreamWin32::~FileStreamWin32() {
}

CORE_RESULT FileStreamWin32::Init(FileManagerWin32 * manager, const str_t filePath, const sint32_t options, const uint32_t priority) {
    const bool read = ((options & FILE_OPT_READ) != 0);
    const bool write = ((options & FILE_OPT_WRITE) != 0);

    DWORD desiredAccess = 0;
    DWORD sharedModel = 0;
    DWORD creationDisposition = 0;
    DWORD flagsAndAttr = 0;
    
    if (read) {
        desiredAccess |= GENERIC_READ;
    }
    if (write) {
        desiredAccess |= GENERIC_WRITE;
    }

    if (read && write) {
        creationDisposition |= OPEN_ALWAYS;
    } else if (read) {
        creationDisposition |= OPEN_EXISTING;
    } else if (write) {
        creationDisposition |= OPEN_ALWAYS;
    }

    flagsAndAttr |= FILE_FLAG_OVERLAPPED;   // async access

    if (options & FILE_OPT_SHARE_READ) {
        sharedModel |= FILE_SHARE_READ;
    }
    if (options & FILE_OPT_SHARE_WRITE) {
        sharedModel |= FILE_SHARE_WRITE;
    }

    if (options & FILE_OPT_RANDOM_ACCESS) {
        flagsAndAttr |= FILE_FLAG_RANDOM_ACCESS;
    } else if (options & FILE_OPT_SEQUENTIAL_SCAN) {
        flagsAndAttr |= FILE_FLAG_SEQUENTIAL_SCAN;
    }
    if (options & FILE_OPT_DELETE_ON_CLOSE) {
        flagsAndAttr |= FILE_FLAG_DELETE_ON_CLOSE;
    }

    bool isAttrNormal = true;
    if (options & FILE_OPT_ATTR_ENCRYPTED) {
        flagsAndAttr |= FILE_ATTRIBUTE_ENCRYPTED;
        isAttrNormal = false;
    }
    if (options & FILE_OPT_ATTR_HIDDEN) {
        flagsAndAttr |= FILE_ATTRIBUTE_HIDDEN;
        isAttrNormal = false;
    }
    if (options & FILE_OPT_ATTR_READONLY) {
        flagsAndAttr |= FILE_ATTRIBUTE_READONLY;
        isAttrNormal = false;
    }
    if (options & FILE_OPT_ATTR_TEMPORARY) {
        flagsAndAttr |= FILE_ATTRIBUTE_TEMPORARY;
        flagsAndAttr |= FILE_ATTRIBUTE_NOT_CONTENT_INDEXED; // インデックスサービス対象外
        isAttrNormal = false;
    }
    if (isAttrNormal) {
        flagsAndAttr |= FILE_ATTRIBUTE_NORMAL;
    }

    HANDLE hFile = CreateFile(
        filePath,
        desiredAccess,
        sharedModel,
        NULL,   // security attributes
        creationDisposition,
        flagsAndAttr,
        NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) {
        return ERR_INVALID;
    }

    fileHandle_ = hFile;
    manager_ = manager;
    options_ = options;
    priority_ = priority;
    seekOffset_ = 0;

    LARGE_INTEGER fSize;
    GetFileSizeEx(fileHandle_, &fSize);
    length_ = static_cast<uint64_t>(fSize.QuadPart);

    // デバッグ用
    StringUtility::Copy(name_, ARRAY_SIZEOF(name_), filePath);

    for (sint32_t i = 0; i < MAX_JOB_NODES; ++i) {
        FileStreamJobNodeWin32 * job = &jobNode_[i];

        job->opFlag = FILE_STREAM_JOB_FLAG_UNKNOWN;
        job->fileHandle = NULL;
        job->finishedEventHandle = NULL;
        job->priority = priority;
    }

    return R_OK;
}

CORE_RESULT FileStreamWin32::DoRead(void * dst, const sint64_t dstSize, const sint64_t dstOffset, const sint64_t readSize, FileStreamEndCallback callback, void * callbackParms) {
    assert((options_ & FILE_OPT_READ) != 0);
    assert(dstOffset > dstSize);

    FileStreamJobNodeWin32 * job = GetJobNode();
    if (job) {
        job->opFlag = FILE_STREAM_JOB_FLAG_READ;
        job->seekOffset = seekOffset_;
        job->callback = callback;
        job->callbackParms = callbackParms;
        
        job->buffer = dst;
        job->bufferSize = dstSize;
        job->bufferOffset = dstOffset;
        job->opSize = readSize;
        job->name = name_;

        // 読み込み分シークを更新
        seekOffset_ += readSize;

        // JobQueueへ
        if (manager_->PushJob(job)) {
            manager_->RaiseWorkerThread();
            return R_OK;
        }
    }
    return R_FAILED;
}

CORE_RESULT FileStreamWin32::DoWrite(const void * src, const sint64_t srcSize, const sint64_t srcOffset, const sint64_t writeSize, FileStreamEndCallback callback, void * callbackParms) {
    assert((options_ & FILE_OPT_WRITE) != 0);

    FileStreamJobNodeWin32 * job = GetJobNode();
    if (job) {
        job->opFlag = FILE_STREAM_JOB_FLAG_WRITE;
        job->seekOffset = seekOffset_;
        job->callback = callback;
        job->callbackParms = callbackParms;
        
        job->buffer = const_cast<void *>(src);
        job->bufferSize = srcSize;
        job->bufferOffset = srcOffset;
        job->opSize = writeSize;
        job->name = name_;
        
        // 書き込み分シークを更新
        seekOffset_ += writeSize;

        // JobQueueへ
        if (manager_->PushJob(job)) {
            manager_->RaiseWorkerThread();
            return R_OK;
        }
    }
    return R_FAILED;
}

void FileStreamWin32::DoClose() {
    // Jobの終了を待つ
    Wait(CORE_WAIT_INFINITE);
    
    // バッファをすべて書き出す
    FlushFileBuffers(fileHandle_);
    
    // ファイルを閉じる
    CloseHandle(fileHandle_);

    // マネージャへ返す
    manager_->PushFileStream(this);
}

CORE_RESULT FileStreamWin32::DoSeek(const sint64_t offset, const SreamSeekOrigin origin) {
    switch (origin) {
    case SEEK_ORIGIN_BEGIN:
        seekOffset_ = Clamp(offset, 0LL, length_);
        break;
    case SEEK_ORIGIN_CURRENT:
        seekOffset_ = Clamp(seekOffset_ + offset, 0LL, length_);
        break;
    case SEEK_ORIGIN_END:
        seekOffset_ = Clamp(length_ + offset, 0LL, length_);
    }
    return R_OK;
}

sint64_t FileStreamWin32::DoLength() {
    return length_;
}

CORE_RESULT FileStreamWin32::DoWait(const uint32_t milliseconds) {
    sint32_t numEvents = 0;
    HANDLE finishEvents[MAX_JOB_NODES];
    HANDLE overlappedEvents[MAX_JOB_NODES];
    for (sint32_t i = 0; i < MAX_JOB_NODES; ++i) {
        if (jobNode_[i].fileHandle != NULL) {
            finishEvents[numEvents] = jobNode_[i].finishedEventHandle;
            overlappedEvents[numEvents] = jobNode_[i].overlapped.hEvent;
            jobNode_[i].fileHandle = NULL;
            jobNode_[i].finishedEventHandle = NULL;
            jobNode_[i].overlapped.hEvent = NULL;

            ++numEvents;
        }
    }

    CORE_RESULT result = R_OK;
    if (numEvents > 0) {
        DWORD r = ::WaitForMultipleObjects(numEvents, finishEvents, TRUE, milliseconds);
        
        if (r == WAIT_FAILED) {
            result = ERR_FAILED;
        } else if (WAIT_OBJECT_0 <= r && r < WAIT_OBJECT_0 + numEvents) {
            result = R_OK;
        } else if (WAIT_ABANDONED_0 <= r && r < WAIT_ABANDONED_0 + numEvents) {
            return R_OK;
        } else if (r == WAIT_TIMEOUT) {
            result = ERR_TIMEOUT;
        } else {
            result = ERR_UNKNOWN;
        }

        for (sint32_t i = 0; i < numEvents; ++i) {
            CloseHandle(finishEvents[i]);
            CloseHandle(overlappedEvents[i]);
        }
    }
    return result;
}

FileStreamJobNodeWin32 * FileStreamWin32::GetJobNode() {
    FileStreamJobNodeWin32 * job = nullptr;
    for (sint32_t i = 0; i < MAX_JOB_NODES; ++i) {
        if (jobNode_[i].overlapped.hEvent == NULL) {
            job = &jobNode_[i];
            jobNode_[i].fileHandle = fileHandle_;
            jobNode_[i].overlapped.hEvent = CreateEvent(NULL, FALSE, TRUE, _T("file stream job"));
            break;
        }
    }
    return job;
}

//---------------------------------------------------
// FileStreamJobNodeWin32
//---------------------------------------------------
FileStreamJobNodeWin32::FileStreamJobNodeWin32()
: callback(nullptr)
, callbackParms(nullptr)
, fileHandle(nullptr)
, overlapped()
, opFlag(FILE_STREAM_JOB_FLAG_UNKNOWN)
, buffer(nullptr)
, bufferSize(0)
, bufferOffset(0)
, opSize(0)
, seekOffset(0) {
    MemSet(&overlapped, 0, sizeof(overlapped));
}

FileStreamJobNodeWin32::~FileStreamJobNodeWin32() {
}

sint32_t FileStreamJobNodeWin32::DoWork() {
    sint32_t result = R_OK;
    switch (opFlag) {
    case FILE_STREAM_JOB_FLAG_READ:
        result = ReadStream();
        break;
    case FILE_STREAM_JOB_FLAG_WRITE:
        result = WriteStream();
        break;
    default:
        result = ERR_INVALID;
    }
    
    // 終了イベント通知
    ::SetEvent(finishedEventHandle);

    return result;
}

namespace {
static const uint64_t s_maxReadSize  = 2ULL * 1024ULL * 1024ULL * 1024ULL;    // 2 GiB
static const uint64_t s_maxWriteSize = 2ULL * 1024ULL * 1024ULL * 1024ULL;    // 2 GiB
}

sint32_t FileStreamJobNodeWin32::ReadStream() {
    LPVOID dst = reinterpret_cast<LPVOID>((uintptr_t)buffer + bufferOffset);
    uint64_t dstSize = Min(opSize, bufferSize - bufferOffset);
                
    sint32_t endCode = R_OK;
    uint64_t numTransfered = 0;
    for (;;) {
        DWORD readSize = static_cast<DWORD>(Min(s_maxReadSize, dstSize - numTransfered));
        if (readSize > 0) {
            LPVOID base = reinterpret_cast<LPVOID>((uintptr_t)dst + numTransfered);
            sint64_t offset = seekOffset + numTransfered;

            ::ResetEvent(overlapped.hEvent);
            overlapped.Offset = static_cast<DWORD>(offset);
            overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);

            BOOL r = ::ReadFile(fileHandle, base, readSize, NULL, &overlapped);
            if (!r) {
                DWORD err = GetLastError();
                if (err == ERROR_IO_PENDING) {
                    DWORD readBytes = 0;
                    GetOverlappedResult(fileHandle, &overlapped, &readBytes, TRUE);
                    numTransfered += readBytes;
                } else if (err == ERROR_HANDLE_EOF) {
                    endCode = ERR_EOF;
                    break;
                } else {
                    endCode = ERR_INVALID;
                    break;
                }
            }
        } else {
            endCode = R_OK;
            break;
        }
    }

    // 完了コールバック呼び出し
    if (callback) {
        callback(endCode, numTransfered, callbackParms);
    }

    return endCode;
}

sint32_t FileStreamJobNodeWin32::WriteStream() {
    LPVOID src = reinterpret_cast<LPVOID>((uintptr_t)buffer + bufferOffset);
    uint64_t srcSize = Min(bufferSize, bufferSize - bufferOffset);
                
    sint32_t endCode = R_OK;
    uint64_t numTransfered = 0;
    for (;;) {
        DWORD writeLen = static_cast<DWORD>(Min(s_maxWriteSize, srcSize - numTransfered));
        if (writeLen > 0) {
            LPVOID base = reinterpret_cast<LPVOID>((uintptr_t)src + numTransfered);
            sint64_t offset = seekOffset + numTransfered;

            ::ResetEvent(overlapped.hEvent);
            overlapped.Offset = static_cast<DWORD>(offset);
            overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);

            BOOL r = ::WriteFile(fileHandle, base, writeLen, NULL, &overlapped);
            if (!r) {
                DWORD err = GetLastError();
                if (err == ERROR_IO_PENDING) {
                    DWORD writeBytes = 0;
                    GetOverlappedResult(fileHandle, &overlapped, &writeBytes, TRUE);
                    numTransfered += writeBytes;
                } else if (err == ERROR_HANDLE_EOF) {
                    endCode = ERR_EOF;
                    break;
                } else {
                    endCode = ERR_INVALID;
                    break;
                }
            }
        } else {
            endCode = R_OK;
            break;
        }
    }

    // 完了コールバック呼び出し
    if (callback) {
        callback(endCode, numTransfered, callbackParms);
    }

    return endCode;
}

}   // namespace "core"
