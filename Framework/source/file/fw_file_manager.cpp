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
    bool                        finished;
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
class FileIOThread : public Thread {
public:
    FileIOSharedBuffer *    sharedBuffer;


    virtual sint32_t Invoke(void * userArgs) FW_OVERRIDE {
        
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
                assert(result != FW_OK);
            }

            // ファイルアクセス
            if ((cmd.flags & FileIOCommand::kFlagFileRead) != 0) {
                sint32_t result = FileRead(cmd.fp, cmd.rwBuffer, cmd.rwSize, nullptr);
                assert(result == FW_OK);
            } else if ((cmd.flags & FileIOCommand::kFlagFileWrite) != 0) {
                sint32_t result = FileWrite(cmd.fp, cmd.rwBuffer, cmd.rwSize, nullptr);
                assert(result == FW_OK);
            }

            // 終了通知
            if ((cmd.flags & FileIOCommand::kFlagNotification) != 0) {
                {
                    std::lock_guard<std::mutex> lock(cmd.notification->mtx);
                    cmd.notification->finished = true;
                }
                cmd.notification->cv.notify_all();
            }
        }

        return 0;
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

        ThreadDesc threadDesc;
        threadDesc.Init();
        threadDesc.userArgs = nullptr; //! @todo
        string::Copy(threadDesc.name, ARRAY_SIZEOF(threadDesc.name), _T("FileIO Thread"));

        fileIOThread.sharedBuffer = &this->sharedBuffer;
        fileIOThread.StartWorker(&threadDesc);
    }

    // 破棄
    virtual void DoShutdown() FW_OVERRIDE {
        fileIOThread.Shutdown();
    }

    // ファイルストリームを開く
    virtual FileReadStream * DoFileReadOpen(const str_t filePath, const sint32_t options, const uint32_t priority) FW_OVERRIDE;
    virtual FileWriteStream * DoFileWriteOpen(const str_t filePath, const sint32_t options, const uint32_t priority) FW_OVERRIDE;

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

/**
 * @class FileReadStreamImpl
 */
class FileReadStreamImpl : public FileReadStream {
public:
    char_t      filePath[Path::kMaxPathLen + 1];
    file_t      fileHandle;
    uint32_t    priority;

    sint64_t    seekOffset;
    SeekOrigin  seekOrigin;
    bool        updateFileSeek;

    FileIOSharedBuffer *        sharedBuffer;
    vector<FileIOCommand>       localBuffer;
    FileIONotification          finishNotification;


    // ファイルを閉じる
    virtual void DoClose() FW_OVERRIDE {
        Wait();
        errorCode = FileClose(fileHandle);
    }

    // ファイルの長さを取得
    virtual uint64_t DoLength() FW_OVERRIDE {
        uint64_t fileLength = 0;
        errorCode = GetFileLength(filePath, &fileLength);

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

    // 処理を送出する
    virtual void DoSubmit() FW_OVERRIDE {
        auto & cmd = localBuffer.back();
        cmd.flags |= FileIOCommand::kFlagNotification;
        cmd.notification = &finishNotification;

        {
            // 共有バッファのキューにコマンドを積む
            std::lock_guard<std::mutex> lock(sharedBuffer->commandQueueMutex);

            // 優先度でソート
            auto itr = std::lower_bound(sharedBuffer->commandQueue.begin(), sharedBuffer->commandQueue.end(),
                localBuffer.front(), [](const FileIOCommand & a, const FileIOCommand & b) { return a.priority - b.priority; });

            // 見つかった位置に全て挿入
            sharedBuffer->commandQueue.insert(itr, localBuffer.begin(), localBuffer.end());
        }

        // 送ったので消しておく
        localBuffer.clear();
    }

    // 実行中の処理が完了するまで待つ
    virtual sint32_t DoWait(const uint32_t milliseconds) FW_OVERRIDE {
        {
            std::unique_lock<std::mutex> lock(finishNotification.mtx);
            finishNotification.cv.wait(lock, [this]() { return finishNotification.finished; });
        }
        return FW_OK;
    }


    FileReadStreamImpl() {
        seekOrigin = kSeekOriginCurrent;
        seekOffset = 0;
        updateFileSeek = true;
    }
};

/**
 * @class FileWriteStreamImpl
 */
class FileWriteStreamImpl : public FileWriteStream {
public:
    char_t      filePath[Path::kMaxPathLen + 1];
    file_t      fileHandle;
    sint32_t    priority;

    sint64_t    seekOffset;
    SeekOrigin  seekOrigin;
    bool        updateFileSeek;

    FileIOSharedBuffer *        sharedBuffer;
    vector<FileIOCommand>       localBuffer;
    FileIONotification          finishNotification;


    // ファイルを閉じる
    virtual void DoClose() FW_OVERRIDE {
        Wait();
        errorCode = FileClose(fileHandle);
    }

    // ファイルの長さを取得
    virtual uint64_t DoLength() FW_OVERRIDE {
        uint64_t fileLength = 0;
        errorCode = GetFileLength(filePath, &fileLength);

        return fileLength;
    }

    // ファイルの位置を移動する
    virtual void DoSeek(const sint64_t offset, const SeekOrigin origin) FW_OVERRIDE {
        seekOffset = offset;
        seekOrigin = origin;
    }

    // ファイルへ書き込む
    virtual sint32_t DoWrite(const void * src, const sint64_t srcSize, const sint64_t writeSize) FW_OVERRIDE {
        FileIOCommand cmd;
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

        {
            // 共有バッファのキューにコマンドを積む
            std::lock_guard<std::mutex> lock(sharedBuffer->commandQueueMutex);

            // 優先度でソート
            auto itr = std::lower_bound(sharedBuffer->commandQueue.begin(), sharedBuffer->commandQueue.end(),
                localBuffer.front(), [](const FileIOCommand & a, const FileIOCommand & b) { return a.priority - b.priority; });

            // 見つかった位置に全て挿入
            sharedBuffer->commandQueue.insert(itr, localBuffer.begin(), localBuffer.end());
        }

        // 送ったので消しておく
        localBuffer.clear();
    }

    // 実行中のジョブが完了するまで待つ
    virtual sint32_t DoWait(const uint32_t milliseconds) FW_OVERRIDE {
        {
            std::unique_lock<std::mutex> lock(finishNotification.mtx);
            finishNotification.cv.wait(lock, [this]() { return finishNotification.finished; });
        }
        return FW_OK;
    }


    FileWriteStreamImpl() {
        seekOrigin = kSeekOriginCurrent;
        seekOffset = 0;
        updateFileSeek = true;
    }
};



// ファイルストリームを開く
FileReadStream * FileManagerImpl::DoFileReadOpen(const str_t filePath, const sint32_t options, const uint32_t priority) {
    file_t fp;
    sint32_t result = FileOpen(filePath, options | kFileOptAccessRead, fp);
    if (result != FW_OK) {
        return nullptr;
    }

    FileReadStreamImpl * stream = FwNew<FileReadStreamImpl>();
    stream->fileHandle = fp;
    stream->priority = priority;
    string::Copy(stream->filePath, ARRAY_SIZEOF(stream->filePath), filePath);

    return stream;
}

// ファイルストリームを開く
FileWriteStream * FileManagerImpl::DoFileWriteOpen(const str_t filePath, const sint32_t options, const uint32_t priority) {
    file_t fp;
    sint32_t result = FileOpen(filePath, options | kFileOptAccessWrite, fp);
    if (result != FW_OK) {
        return nullptr;
    }

    FileWriteStreamImpl * stream = FwNew<FileWriteStreamImpl>();
    stream->fileHandle = fp;
    stream->priority = priority;
    string::Copy(stream->filePath, ARRAY_SIZEOF(stream->filePath), filePath);

    return stream;
}

END_NAMESPACE_NONAME

// 生成
FileManager * CreateFileManager(FileManagerDesc * desc) {
    FileManagerImpl * manager = FwNew<FileManagerImpl>();
    manager->Init(desc);

    return manager;
}

END_NAMESPACE_FW
