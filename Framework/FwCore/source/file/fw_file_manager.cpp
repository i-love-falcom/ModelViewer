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
    std::mutex                  _mutexFileIO;
    std::condition_variable     _condFileIO;
    bool                        _ready;

    void Reset() {
        _ready = false;
    }

    void Notify() {
        {
            std::lock_guard<std::mutex> lock(_mutexFileIO);
            _ready = true;
        }
        _condFileIO.notify_all();
    }

    void Wait() {
        std::unique_lock<std::mutex> lock(_mutexFileIO);
        _condFileIO.wait(lock, [this]() { return _ready; });
    }

    bool WaitFor(const uint32_t milliseconds) {
        std::unique_lock<std::mutex> lock(_mutexFileIO);
        return _condFileIO.wait_for(lock, std::chrono::milliseconds(milliseconds), [this]() { return _ready; });
    }
};

/**
 * @struct FwFileIOCommand
 */
struct FwFileIOCommand {
    enum {
        kFlagFileRead       = FW_BIT32(0),
        kFlagFileWrite      = FW_BIT32(1),
        kFlagFileSeek       = FW_BIT32(2),
        kFlagNotification   = FW_BIT32(3),
    };

    FwFile          _fp;
    sint32_t        _flags;
    uint32_t        _priority;
    FwSeekOrigin    _seekOrigin;
    sint64_t        _seekOffset;

    void *      _rwBuffer;
    uint64_t    _rwSize;

    FwFileIONotification * notification;
};


/**
 * @struct FwFileIOSharedBuffer
 */
struct FwFileIOSharedBuffer {
    deque<FwFileIOCommand>  _commandQueue;
    std::mutex              _commandQueueMutex;
};

/**
 * @class FwFileIOThread
 */
class FwFileIOThread : public FwThread {
public:
    FwFileIOSharedBuffer *  _sharedBuffer;


    virtual sint32_t ThreadFunc(void * userArgs) FW_OVERRIDE {
        
        while (true) {
            FwFileIOCommand cmd;
            
            // キューからコマンドを取得
            bool foundNewCommand = false;
            if (!_sharedBuffer->_commandQueue.empty()) {
                std::lock_guard<std::mutex> lock(_sharedBuffer->_commandQueueMutex);
                if (!_sharedBuffer->_commandQueue.empty()) {
                    cmd = _sharedBuffer->_commandQueue.front();
                    _sharedBuffer->_commandQueue.pop_front();
                    foundNewCommand = true;
                }
            }
            if (!foundNewCommand) {
                break;
            }

            // 読み込み／書き込み位置を移動
            if ((cmd._flags & FwFileIOCommand::kFlagFileSeek) != 0) {
                sint32_t result = FwFileSeek(cmd._fp, cmd._seekOffset, cmd._seekOrigin);
                FwAssert(result != FW_OK);
            }

            // ファイルアクセス
            if ((cmd._flags & FwFileIOCommand::kFlagFileRead) != 0) {
                sint32_t result = FwFileRead(cmd._fp, cmd._rwBuffer, cmd._rwSize, nullptr);
                FwAssert(result == FW_OK);
            } else if ((cmd._flags & FwFileIOCommand::kFlagFileWrite) != 0) {
                sint32_t result = FwFileWrite(cmd._fp, cmd._rwBuffer, cmd._rwSize, nullptr);
                FwAssert(result == FW_OK);
            }

            // 終了通知
            if ((cmd._flags & FwFileIOCommand::kFlagNotification) != 0) {
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
        cmd._fp              = fileHandle;
        cmd._flags           = FwFileIOCommand::kFlagFileRead;
        cmd._priority        = priority;
        cmd._seekOrigin      = seekOrigin;
        cmd._seekOffset      = seekOffset;
        cmd._rwBuffer        = dst;
        cmd._rwSize          = readSize;

        if (updateFileSeek) {
            cmd._flags |= FwFileIOCommand::kFlagFileSeek;
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
        cmd._fp              = fileHandle;
        cmd._flags           = FwFileIOCommand::kFlagFileWrite;
        cmd._seekOrigin      = seekOrigin;
        cmd._seekOffset      = seekOffset;
        cmd._rwBuffer        = const_cast<void *>(src);
        cmd._rwSize          = writeSize;

        if (updateFileSeek) {
            cmd._flags |= FwFileIOCommand::kFlagFileSeek;
            updateFileSeek = false;
        }

        return FW_OK;
    }

    // 処理を送出する
    virtual void DoSubmit() FW_OVERRIDE {
        auto & cmd = localBuffer.back();
        cmd._flags |= FwFileIOCommand::kFlagNotification;
        cmd.notification = &finishNotification;

        finishNotification.Reset();

        FwFileIOSharedBuffer * sharedBuffer = fileIOThread->_sharedBuffer;
        {
            // 共有バッファのキューにコマンドを積む
            std::lock_guard<std::mutex> lock(sharedBuffer->_commandQueueMutex);

            // 優先度でソート
            auto itr = std::lower_bound(sharedBuffer->_commandQueue.begin(), sharedBuffer->_commandQueue.end(),
                localBuffer.front(), [](const FwFileIOCommand & a, const FwFileIOCommand & b) { return a._priority - b._priority; });

            // 見つかった位置に全て挿入
            sharedBuffer->_commandQueue.insert(itr, localBuffer.begin(), localBuffer.end());
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
        FwPath::GetCurrentDir(basePath, FW_ARRAY_SIZEOF(basePath));

        FwThreadDesc threadDesc;
        threadDesc.Init();
        threadDesc.affinity = desc->_threadAffinity;
        threadDesc.priority = desc->_threadPriority;
        string::Copy(threadDesc.name, FW_ARRAY_SIZEOF(threadDesc.name), _T("FileIO Thread"));

        fileIOThread._sharedBuffer = &this->sharedBuffer;
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
        string::Copy(stream->filePath, FW_ARRAY_SIZEOF(stream->filePath), filePath);

        return stream;
    }

    // 基準となるパスをセット
    virtual void DoSetBasePath(const str_t path) FW_OVERRIDE {
        string::Copy(basePath, FW_ARRAY_SIZEOF(basePath), path);
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
