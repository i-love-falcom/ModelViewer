/**
 * @file fw_file_stream.h
 * @author Kamai Masayoshi
 */
#ifndef FW_FILE_STREAM_H_
#define FW_FILE_STREAM_H_

#include "file/fw_file_types.h"
#include "file/fw_path.h"
#include "misc/fw_noncopyable.h"


BEGIN_NAMESPACE_FW

/**
* @class FileStream
*/
class FileStream : public NonCopyable<FileStream> {
public:
    /**
     * @brief ストリームを閉じる
     */
    FW_INLINE void Close() {
        DoClose();
    }

    /**
     * @brief ファイルの長さを取得
     */
    FW_INLINE uint64_t Length() {
        DoLength();
    }

    /**
     * @brief ファイルの位置を移動する
     */
    FW_INLINE void Seek(const sint64_t offset, const SeekOrigin origin) {
        return DoSeek(offset, origin);
    }

    /**
     * @brief 処理を送出する
     */
    FW_INLINE void Submit() {
        DoSubmit();
    }

    /**
     * @brief 実行中の処理が完了するまで待つ
     */
    FW_INLINE sint32_t Wait(const uint32_t milliseconds = FW_WAIT_INFINITE) {
        return DoWait(milliseconds);
    }

    /**
     * @brief 名前を取得
     */
    FW_INLINE const str_t GetFileName() {
        return fileName;
    }

    /**
     * @brief エラーコードを取得
     */
    FW_INLINE sint32_t GetError() {
        return errorCode;
    }


protected:
    /**
     * @brief ファイルを閉じる
     */
    virtual void DoClose() = 0;

    /**
     * @brief ファイルの長さを取得
     */
    virtual uint64_t DoLength() = 0;

    /**
     * @brief ファイルの位置を移動する
     */
    virtual void DoSeek(const sint64_t offset, const SeekOrigin origin) = 0;

    /** 
     * @brief 処理を送出する
     */
    virtual void DoSubmit() = 0;

    /**
     * @brief 実行中のジョブが完了するまで待つ
     */
    virtual sint32_t DoWait(const uint32_t milliseconds) = 0;


   /**
     * @brief コンストラクタ
     */
    FileStream() {
        fileName[0] = _T('\0');
        errorCode = FW_OK;
    }

    /**
     * @brief デストラクタ
     */
    virtual ~FileStream() {
    }


    char_t      fileName[Path::kMaxFNameLen];
    sint32_t    errorCode;
};

/**
 * @class FileReadStream
 */
class FileReadStream : public FileStream {
public:
    /**
     * @brief ファイルを読み込む
     */
    FW_INLINE sint32_t Read(void * dst, const sint64_t dstSize, const sint64_t readSize) {
        return DoRead(dst, dstSize, readSize);
    }

    /**
     * @brief ファイルを読み込む
     */
    FW_INLINE void ReadAndSubmit(void * dst, const sint64_t dstSize, const sint64_t readSize) {
        DoRead(dst, dstSize, readSize);
        Submit();
    }


protected:
    /**
     * @brief ファイルを読み込む
     */
    virtual sint32_t DoRead(void * dst, const sint64_t dstSize, const sint64_t readSize) = 0;

    /**
     * @brief コンストラクタ
     */
    FileReadStream() {
    }

    /**
     * @brief デストラクタ
     */
    virtual ~FileReadStream() {
    }
};

/**
 * @class FileWriteStream
 */
class FileWriteStream : public FileStream {
public:
    /**
     * @brief ファイルへ書き込む
     */
    FW_INLINE sint32_t Write(const void * src, const sint64_t srcSize, const sint64_t writeSize) {
        return DoWrite(src, srcSize, writeSize);
    }
    
    /**
     * @brief ファイルへ書き込む
     */
    FW_INLINE void WriteAndSubmit(const void * src, const sint64_t srcSize, const sint64_t writeSize) {
        DoWrite(src, srcSize, writeSize);
        Submit();
    }


protected:
    /**
     * @brief ファイルへ書き込む
     */
    virtual sint32_t DoWrite(const void * src, const sint64_t srcSize, const sint64_t writeSize) = 0;

    /**
     * @brief コンストラクタ
     */
    FileWriteStream() {
    }

    /**
     * @brief デストラクタ
     */
    virtual ~FileWriteStream() {
    }
};

END_NAMESPACE_FW

#endif  // FW_FILE_STREAM_H_
