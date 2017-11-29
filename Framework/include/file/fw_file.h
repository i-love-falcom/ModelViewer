/**
 * @file fw_file.h
 * @author Kamai Masayoshi
 */
#ifndef FW_FILE_H_
#define FW_FILE_H_

#include "file/fw_file_types.h"


BEGIN_NAMESPACE_FW

/**
 * @brief ファイルを開く
 * @param[in]      relativePath 相対パス
 * @param[in]      fileName     ファイル名
 * @param[in]      options      FileOptionsを論理和したもの
 * @param[in, out] fp           ファイルディスクリプタ
 */
FW_DLL_FUNC sint32_t FileOpen(const str_t name, const sint32_t options, file_t & fp);

/**
 * @brief ファイルを閉じる
 * @param[in] fp ファイルディスクリプタ
 */
FW_DLL_FUNC sint32_t FileClose(file_t & fp);

/**
 * @brief ファイルを削除
 * @param[in] relativePath 相対パス
 * @param[in] fileName     ファイル名
 */
FW_DLL_FUNC sint32_t FileDelete(const str_t fileName);

/**
 * @brief ファイルを読み込む
 * @param[in]  fp           ファイルディスクリプタ
 * @param[in]  dst          読み込み先バッファ
 * @param[in]  toReadSize   読み込みサイズ
 * @param[out] readSize     読み込まれたサイズを格納する変数へのポインタ
 */
FW_DLL_FUNC sint32_t FileRead(file_t & fp, void * dst, const uint64_t toReadSize, uint64_t * readSize);

/**
 * @brief ファイルへ書き込む
 * @param[in] fp          ファイルディスクリプタ
 * @param[in] src         書き込み元バッファ
 * @param[in] toWriteSize 書き込みサイズ
 * @param[in] writeSize   書き込まれたサイズを格納する変数へのポインタ
 */
FW_DLL_FUNC sint32_t FileWrite(file_t & fp, const void * src, const uint64_t toWriteSize, uint64_t * writeSize);

/**
 * @brief ファイルのサイズを取得する
 * @param[in]  relativePath 相対パス
 * @param[in]  fileName     ファイル名
 * @param[out] length       byte単位の長さ
 */
FW_DLL_FUNC sint32_t GetFileLength(const str_t name, uint64_t * length);

/**
 * @brief ファイルが存在するか調べる
 * @param[in] relativePath 相対パス
 * @param[in] fileName     ファイル名
 */
FW_DLL_FUNC bool IsFileExist(const str_t name);

/**
 * @brief ファイルポインタを移動する
 * @param[in] fp     ファイルディスクリプタ
 * @param[in] offset 基準位置からのオフセット
 * @param[in] origin 基準位置
 */
FW_DLL_FUNC sint32_t FileSeek(file_t & fp, const sint64_t offset, const SeekOrigin origin);

/**
 * @brief キャッシュされたデータをフラッシュする
 * @param[in] fp ファイルディスクリプタ
 */
FW_DLL_FUNC sint32_t FlushFileBuffer(file_t & fp);


/**
 * @brief 標準入出力デバイスハンドルを取得
 */
FW_DLL_FUNC file_t GetStdFileHandle(const StdDeviceHandle handle);


/**
 * @class FileHolder
 */
class FileHolder {
public:
    FW_INLINE file_t & Get() {
        return fp_;
    }

    FW_INLINE const file_t & Get() const {
        return fp_;
    }
    
    FileHolder(file_t & fp)
    : fp_(fp) {
    }

    ~FileHolder() {
        FileClose(fp_);
    }

private:
    file_t fp_;
};

END_NAMESPACE_FW

#endif  // FW_FILE_H_
