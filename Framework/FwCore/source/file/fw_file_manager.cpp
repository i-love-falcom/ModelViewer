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

class FileReadStreamImpl;
class FileWriteStreamImpl;

/**
 * @struct FileIONotification
 */
struct FileIONotification {
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
 * @struct FileIOCommand
 */
struct FileIOCommand {
    enum {
        kFlagFileRead       = BIT32(0),
        kFlagFileWrite      = BIT32(1),
        kFlagFileSeek       = BIT32(2),
        kFlagNotification   = BIT32(3),
    };

    file_t      fp;
    sint32_t    flags;
    uint32_t    priority;
    SeekOrigin  seekOrigin;
    sint64_t    seekOffset;

    void *      rwBuffer;
    uint64_t    rwSize;

    FileIONotification * notification;
};


/**
 * @struct FileIOSharedBuffer
 */
struct FileIOSharedBuffer {
    deque<FileIOCommand>    commandQueue;
    std::mutex              commandQueueMutex;
};

/**
 * @class FileIOThread
 */
class FileIOThread : public FwThread {
public:
    FileIOSharedBuffer *    sharedBuffer;


    virtual sint32_t ThreadFunc(void * userArgs) FW_OVERRIDE {
        
        while (true) {
            FileIOCommand cmd;
            
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
            if ((cmd.flags & FileIOCommand::kFlagFileSeek) != 0) {
                sint32_t result = FileSeek(cmd.fp, cmd.seekOffset, cmd.seekOrigin);
                FwAssert(result != FW_OK);
            }

            // ファイルアクセス
            if ((cmd.flags & FileIOCommand::kFlagFileRead) != 0) {
                sint32_t result = FileRead(cmd.fp, cmd.rwBuffer, cmd.rwSize, nullptr);
                FwAssert(result == FW_OK);
            } else if ((cmd.flags & FileIOCommand::kFlagFileWrite) != 0) {
                sint32_t result = FileWrite(cmd.fp, cmd.rwBuffer, cmd.rwSize, nullptr);
                FwAssert(result == FW_OK);
            }

            // 終了通知
            if ((cmd.flags & FileIOCommand::kFlagNotification) != 0) {
                cmd.notification->Notify();
            }
        }

        return 0;
    }
};

/**
 * @class FileStreamImpl
 */
class FileStreamImpl : public FileStream {
public:
    char_t      filePath[Path::kMaxPathLen + 1];
    file_t      fileHandle;
    uint32_t    priority;

    sint64_t    seekOffset;
    SeekOrigin  seekOrigin;
    bool        updateFileSeek;

    vector<FileIOCommand>   localBuffer;
    FileIONotification      finishNotification;

    FileIOThread *          fileIOThread;


    // ファイルを閉じる
    virtual void DoClose() FW_OVERRIDE {
        SubmitDone();
        FileClose(fileHandle);

        // 自身を破棄
        FwDelete<FileStreamImpl>(this);
    }

    // ファイルの長さを取得
    virtual uint64_t DoLength() FW_OVERRIDE {
        uint64_t fileLength = 0;
        GetFileLength(filePath, &fileLength);

        return fileLength;
    }

    // ファイルの位置を移動する
    virtual void DoSeek(const sint64_t offset, const SeekOrigin origin) FW_OVERRIDE {
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

        FileIOCommand & cmd = localBuffer.append();
        cmd.fp              = fileHandle;
        cmd.flags           = FileIOCommand::kFlagFileRead;
        cmd.priority        = priority;
        cmd.seekOrigin      = seekOrigin;
        cmd.seekOffset      = seekOffset;
        cmd.rwBuffer        = dst;
        cmd.rwSize          = readSize;

        if (updateFileSeek) {
            cmd.flags |= FileIOCommand::kFlagFileSeek;
            updateFileSeek = false;
        }

        return FW_OK;
    }

    // ファイルへ書き込む
    virtual sint32_t DoWrite(const void * src, const sint64_t srcSize, const sint64_t writeSize) FW_OVERRIDE {
        if ((fileHandle.options & kFileOptAccessWrite) == 0) {
            return ERR_INVALID;
        }

        FileIOCommand & cmd = localBuffer.append();
        cmd.fp              = fileHandle;
        cmd.flags           = FileIOCommand::kFlagFileWrite;
        cmd.seekOrigin      = seekOrigin;
        cmd.seekOffset      = seekOffset;
        cmd.rwBuffer        = const_cast<void *>(src);
        cmd.rwSize          = writeSize;

        if (updateFileSeek) {
            cmd.flags |= FileIOCommand::kFlagFileSeek;
            updateFileSeek = false;
        }

        return FW_OK;
    }

    // 処理を送出する
    virtual void DoSubmit() FW_OVERRIDE {
        auto & cmd = localBuffer.back();
        cmd.flags |= FileIOCommand::kFlagNotification;
        cmd.notification = &finishNotification;

        finishNotification.Reset();

        FileIOSharedBuffer * sharedBuffer = fileIOThread->sharedBuffer;
        {
            // 共有バッファのキューにコマンドを積む
            std::lock_guard<std::mutex> lock(sharedBuffer->commandQueueMutex);

            // 優先度でソート
            auto itr = std::lower_bound(sharedBuffer->commandQueue.begin(), sharedBuffer->commandQueue.end(),
                localBuffer.front(), [](const FileIOCommand & a, const FileIOCommand & b) { return a.priority - b.priority; });

            // 見つかった位置に全て挿入
            sharedBuffer->commandQueue.insert(itr, localBuffer.begin(), localBuffer.end());
        }

        // スレッドを起こす
        fileIOThread->RestartThread();

        // 送ったので消しておく
        localBuffer.clear();
    }

    // 実行中の処理が完了するまで待つ
    virtual sint32_t DoSubmitDone(const uint32_t milliseconds) FW_OVERRIDE {
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
class FileManagerImpl : public FileManager {
public:
    // 初期化
    void Init(FileManagerDesc * desc) {
        Path::GetCurrentDir(basePath, ARRAY_SIZEOF(basePath));

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
    virtual FileStream * DoFileStreamOpen(const str_t filePath, const sint32_t options, const uint32_t priority) FW_OVERRIDE {
        file_t fp;
        sint32_t result = FileOpen(filePath, options, fp);
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
    char_t  basePath[Path::kMaxPathLen + 1];

    FileIOSharedBuffer  sharedBuffer;
    FileIOThread        fileIOThread;
};

END_NAMESPACE_NONAME

// 生成
FileManager * CreateFileManager(FileManagerDesc * desc) {
    FileManagerImpl * manager = FwNew<FileManagerImpl>();
    manager->Init(desc);

    return manager;
}

END_NAMESPACE_FW
