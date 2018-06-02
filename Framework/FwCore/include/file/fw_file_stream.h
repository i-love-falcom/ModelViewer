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
class FwFileStream : public NonCopyable<FwFileStream> {
public:
    /**
     * @brief ストリームを閉じて自身を破棄
     */
    FW_INLINE void Close() {
        DoClose();
    }

    /**
     * @brief ファイルを読み込む
     */
    FW_INLINE sint32_t Read(void * dst, const sint64_t dstSize, const sint64_t readSize) {
        return DoRead(dst, dstSize, readSize);
    }

    /**
     * @brief ファイルへ書き込む
     */
    FW_INLINE sint32_t Write(const void * src, const sint64_t srcSize, const sint64_t writeSize) {
        return DoWrite(src, srcSize, writeSize);
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
    FW_INLINE void Seek(const sint64_t offset, const FwSeekOrigin origin) {
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


protected:
    /**
     * @brief ファイルを閉じる
     */
    virtual void DoClose() = 0;

    /**
     * @brief ファイルを読み込む
     */
    virtual sint32_t DoRead(void * dst, const sint64_t dstSize, const sint64_t readSize) = 0;

    /**
     * @brief ファイルへ書き込む
     */
    virtual sint32_t DoWrite(const void * src, const sint64_t srcSize, const sint64_t writeSize) = 0;

    /**
     * @brief ファイルの長さを取得
     */
    virtual uint64_t DoLength() = 0;

    /**
     * @brief ファイルの位置を移動する
     */
    virtual void DoSeek(const sint64_t offset, const FwSeekOrigin origin) = 0;

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
    FwFileStream() {
        fileName[0] = _T('\0');
    }

    /**
     * @brief デストラクタ
     */
    virtual ~FwFileStream() {
    }


    char_t  fileName[FwPath::kMaxFileNameLen];
};

END_NAMESPACE_FW

#endif  // FW_FILE_STREAM_H_
