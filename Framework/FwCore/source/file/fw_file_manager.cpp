/**
 * @file fw_file_manager.cpp
 * @author Kamai Masayoshi
 */
#include "precompiled.h"
#include "file/fw_file_manager.h"
#include "file/fw_file_stream.h"
#include "file/fw_file.h"
#include "core/fw_thread.h"
#include "container/fw_deque.h"
#include "container/fw_vector.h"


BEGIN_NAMESPACE_FW
BEGIN_NAMESPACE_NONAME

/**
 * @struct FwFileIONotification
 */
struct FwFileIONotification {
    std::mutex                  mtx;
    std::condition_variable     cv;
    bool                        ready;

    void Reset() {
        ready = false;
    }

    void Notify() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            ready = true;
        }
        cv.notify_all();
    }

    void Wait() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]() { return ready; });
    }

    bool WaitFor(const uint32_t milliseconds) {
        std::unique_lock<std::mutex> lock(mtx);
        return cv.wait_for(lock, std::chrono::milliseconds(milliseconds), [this]() { return ready; });
    }
};

/**
 * @struct FwFileIOCommand
 */
struct FwFileIOCommand {
    enum {
        kFlagFileRead       = BIT32(0),
        kFlagFileWrite      = BIT32(1),
        kFlagFileSeek       = BIT32(2),
        kFlagNotification   = BIT32(3),
    };

    FwFile      fp;
    sint32_t    flags;
    uint32_t    priority;
    FwSeekOrigin  seekOrigin;
    sint64_t    seekOffset;

    void *      rwBuffer;
    uint64_t    rwSize;

    FwFileIONotification * notification;
};


/**
 * @struct FwFileIOSharedBuffer
 */
struct FwFileIOSharedBuffer {
    deque<FwFileIOCommand>  commandQueue;
    std::mutex              commandQueueMutex;
};

/**
 * @class FwFileIOThread
 */
class FwFileIOThread : public FwThread {
public:
    FwFileIOSharedBuffer *  sharedBuffer;


    virtual sint32_t ThreadFunc(void * userArgs) FW_OVERRIDE {
        
        while (true) {
            FwFileIOCommand cmd;
            
            // キューからコマンドを取得
            bool foundNewCommand = false;
            if (!sharedBuffer->commandQueue.empty()) {
                std::lock_guard<std::mutex> lock(sharedBuffer->commandQueueMutex);
                if (!sharedBuffer->commandQueue.empty()) {
                    cmd = sharedBuffer->commandQueue.front();
                    sharedBuffer->commandQueue.pop_front();
                    foundNewCommand = true;
                }
            }
            if (!foundNewCommand) {
                break;
            }

            // 読み込み／書き込み位置を移動
            if ((cmd.flags & FwFileIOCommand::kFlagFileSeek) != 0) {
                sint32_t result = FwFileSeek(cmd.fp, cmd.seekOffset, cmd.seekOrigin);
                FwAssert(result != FW_OK);
            }

            // ファイルアクセス
            if ((cmd.flags & FwFileIOCommand::kFlagFileRead) != 0) {
                sint32_t result = FwFileRead(cmd.fp, cmd.rwBuffer, cmd.rwSize, nullptr);
                FwAssert(result == FW_OK);
            } else if ((cmd.flags & FwFileIOCommand::kFlagFileWrite) != 0) {
                sint32_t result = FwFileWrite(cmd.fp, cmd.rwBuffer, cmd.rwSize, nullptr);
                FwAssert(result == FW_OK);
            }

            // 終了通知
            if ((cmd.flags & FwFileIOCommand::kFlagNotification) != 0) {
                cmd.notification->Notify();
            }
        }

        return 0;
    }
};

/**
 * @class FileStreamImpl
 */
class FileStreamImpl : public FwFileStream {
public:
    char_t      filePath[FwPath::kMaxPathLen + 1];
    FwFile      fileHandle;
    uint32_t    priority;

    sint64_t        seekOffset;
    FwSeekOrigin    seekOrigin;
    bool            updateFileSeek;

    vector<FwFileIOCommand>   localBuffer;
    FwFileIONotification      finishNotification;

    FwFileIOThread *          fileIOThread;


    // ファイルを閉じる
    virtual void DoClose() FW_OVERRIDE {
        Wait();
        FwFileClose(fileHandle);

        // 自身を破棄
        FwDelete<FileStreamImpl>(this);
    }

    // ファイルの長さを取得
    virtual uint64_t DoLength() FW_OVERRIDE {
        uint64_t fileLength = 0;
        FwFileGetLength(fileHandle, &fileLength);

        return fileLength;
    }

    // ファイルの位置を移動する
    virtual void DoSeek(const sint64_t offset, const FwSeekOrigin origin) FW_OVERRIDE {
        if (seekOffset != offset || seekOrigin != origin) {
            seekOffset = offset;
            seekOrigin = origin;

            updateFileSeek = true;
        }
    }

    // ファイルを読み込む
    virtual sint32_t DoRead(void * dst, const sint64_t dstSize, const sint64_t readSize) FW_OVERRIDE {
        if ((fileHandle.options & kFileOptAccessRead) == 0) {
            return ERR_INVALID;
        }

        FwFileIOCommand & cmd = localBuffer.append();
        cmd.fp              = fileHandle;
        cmd.flags           = FwFileIOCommand::kFlagFileRead;
        cmd.priority        = priority;
        cmd.seekOrigin      = seekOrigin;
        cmd.seekOffset      = seekOffset;
        cmd.rwBuffer        = dst;
        cmd.rwSize          = readSize;

        if (updateFileSeek) {
            cmd.flags |= FwFileIOCommand::kFlagFileSeek;
            updateFileSeek = false;
        }

        return FW_OK;
    }

    // ファイルへ書き込む
    virtual sint32_t DoWrite(const void * src, const sint64_t srcSize, const sint64_t writeSize) FW_OVERRIDE {
        if ((fileHandle.options & kFileOptAccessWrite) == 0) {
            return ERR_INVALID;
        }

        FwFileIOCommand & cmd = localBuffer.append();
        cmd.fp              = fileHandle;
        cmd.flags           = FwFileIOCommand::kFlagFileWrite;
        cmd.seekOrigin      = seekOrigin;
        cmd.seekOffset      = seekOffset;
        cmd.rwBuffer        = const_cast<void *>(src);
        cmd.rwSize          = writeSize;

        if (updateFileSeek) {
            cmd.flags |= FwFileIOCommand::kFlagFileSeek;
            updateFileSeek = false;
        }

        return FW_OK;
    }

    // 処理を送出する
    virtual void DoSubmit() FW_OVERRIDE {
        auto & cmd = localBuffer.back();
        cmd.flags |= FwFileIOCommand::kFlagNotification;
        cmd.notification = &finishNotification;

        finishNotification.Reset();

        FwFileIOSharedBuffer * sharedBuffer = fileIOThread->sharedBuffer;
        {
            // 共有バッファのキューにコマンドを積む
            std::lock_guard<std::mutex> lock(sharedBuffer->commandQueueMutex);

            // 優先度でソート
            auto itr = std::lower_bound(sharedBuffer->commandQueue.begin(), sharedBuffer->commandQueue.end(),
                localBuffer.front(), [](const FwFileIOCommand & a, const FwFileIOCommand & b) { return a.priority - b.priority; });

            // 見つかった位置に全て挿入
            sharedBuffer->commandQueue.insert(itr, localBuffer.begin(), localBuffer.end());
        }

        // スレッドを起こす
        fileIOThread->RestartThread();

        // 送ったので消しておく
        localBuffer.clear();
    }

    // 実行中の処理が完了するまで待つ
    virtual sint32_t DoWait(const uint32_t milliseconds) FW_OVERRIDE {
        bool result = true;

        if (milliseconds != FW_WAIT_INFINITE) {
            result = finishNotification.WaitFor(milliseconds);
        } else {
            finishNotification.Wait();
        }
        return result ? FW_OK : ERR_TIMEOUT;
    }


    FileStreamImpl() {
        seekOrigin = kSeekOriginCurrent;
        seekOffset = 0;
        updateFileSeek = true;
    }
};


/**
 * @class FileManagerImpl
 */
class FileManagerImpl : public FwFileManager {
public:
    // 初期化
    void Init(FwFileManagerDesc * desc) {
        FwPath::GetCurrentDir(basePath, ARRAY_SIZEOF(basePath));

        FwThreadDesc threadDesc;
        threadDesc.Init();
        threadDesc.affinity = desc->threadAffinity;
        threadDesc.priority = desc->threadPriority;
        string::Copy(threadDesc.name, ARRAY_SIZEOF(threadDesc.name), _T("FileIO Thread"));

        fileIOThread.sharedBuffer = &this->sharedBuffer;
        fileIOThread.StartWorker(&threadDesc);
    }

    // 破棄
    virtual void DoShutdown() FW_OVERRIDE {
        fileIOThread.Shutdown();
    }

    // ファイルストリームを開く
    virtual FwFileStream * DoFileStreamOpen(const str_t filePath, const sint32_t options, const uint32_t priority) FW_OVERRIDE {
        FwFile fp;
        sint32_t result = FwFileOpen(filePath, options, fp);
        if (result != FW_OK) {
            return nullptr;
        }

        FileStreamImpl * stream = FwNew<FileStreamImpl>();
        stream->fileHandle = fp;
        stream->priority = priority;
        stream->fileIOThread = &fileIOThread;
        string::Copy(stream->filePath, ARRAY_SIZEOF(stream->filePath), filePath);

        return stream;
    }

    // 基準となるパスをセット
    virtual void DoSetBasePath(const str_t path) FW_OVERRIDE {
        string::Copy(basePath, ARRAY_SIZEOF(basePath), path);
    }

    // 基準となるパスを取得
    virtual const str_t DoGetBasePath() FW_OVERRIDE {
        return basePath;
    }

    // コンストラクタ
    FileManagerImpl() {

    }
    
    // デストラクタ
    virtual ~FileManagerImpl() {

    }


private:
    char_t  basePath[FwPath::kMaxPathLen + 1];

    FwFileIOSharedBuffer  sharedBuffer;
    FwFileIOThread        fileIOThread;
};

END_NAMESPACE_NONAME

// 生成
FwFileManager * CreateFileManager(FwFileManagerDesc * desc) {
    FileManagerImpl * manager = FwNew<FileManagerImpl>();
    manager->Init(desc);

    return manager;
}

END_NAMESPACE_FW
