﻿/**
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
sint32_t FileOpen(const str_t name, const sint32_t options, file_t & fp);

/**
 * @brief ファイルを閉じる
 * @param[in] fp ファイルディスクリプタ
 */
sint32_t FileClose(file_t & fp);

/**
 * @brief ファイルを削除
 * @param[in] relativePath 相対パス
 * @param[in] fileName     ファイル名
 */
sint32_t FileDelete(const str_t fileName);

/**
 * @brief ファイルを読み込む
 * @param[in]  fp         ファイルディスクリプタ
 * @param[in]  dst        読み込み先バッファ
 * @param[in]  toReadSize 読み込みサイズ
 * @param[out] readSize   読み込まれたサイズを格納する変数へのポインタ
 */
sint32_t FileRead(file_t & fp, void * dst, const sint64_t toReadSize, sint64_t * readSize);

/**
 * @brief ファイルへ書き込む
 * @param[in] fp          ファイルディスクリプタ
 * @param[in] src         書き込み元バッファ
 * @param[in] toWriteSize 書き込みサイズ
 * @param[in] writeSize   書き込まれたサイズを格納する変数へのポインタ
 */
sint32_t FileWrite(file_t & fp, const void * src, const sint64_t toWriteSize, sint64_t * writeSize);

/**
 * @brief ファイルのサイズを取得する
 * @param[in]  relativePath 相対パス
 * @param[in]  fileName     ファイル名
 * @param[out] length       byte単位の長さ
 */
sint32_t GetFileLength(const str_t name, uint64_t * length);

/**
 * @brief ファイルが存在するか調べる
 * @param[in] relativePath 相対パス
 * @param[in] fileName     ファイル名
 */
bool IsFileExist(const str_t name);

/**
 * @brief ファイルポインタを移動する
 * @param[in] fp     ファイルディスクリプタ
 * @param[in] offset 基準位置からのオフセット
 * @param[in] origin 基準位置
 */
sint32_t FileSeek(file_t & fp, const sint64_t offset, const SeekOrigin origin);

/**
 * @brief キャッシュされたデータをフラッシュする
 * @param[in] fp ファイルディスクリプタ
 */
sint32_t FlushFileBuffer(file_t & fp);


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
