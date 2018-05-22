/**
 * @file fw_file.h
 * @author Kamai Masayoshi
 */
#ifndef FW_FILE_H_
#define FW_FILE_H_

#define FW_FILE_UNKNOWN       (0)
#define FW_FILE_LIBC          (1)
#define FW_FILE_WIN32         (2)

#if FW_BUILD_CONFIG_FORCE_USE_LIBC_FILE
#define FW_FILE             FW_FILE_LIBC
#elif defined(FW_PLATFORM_WIN32)
#define FW_FILE             FW_FILE_WIN32
#else
#define FW_FILE             FW_FILE_LIBC
#endif

#if FW_FILE == FW_FILE_UNKNOWN
#error unsupported platform
#endif

#include "file/fw_file_types.h"


BEGIN_NAMESPACE_FW

class FwFile;
class FwFileHolder;


/**
 * @brief ファイルを開く
 * @param[in]  name     ファイル名
 * @param[in]  options  FileOptionsを論理和したもの
 * @param[out] fp       ファイルディスクリプタ
 */
FW_DLL_FUNC sint32_t FwFileOpen(const str_t name, const uint32_t options, FwFile & fp);

/**
 * @brief ファイルを閉じる
 * @param[in] fp ファイルディスクリプタ
 */
FW_DLL_FUNC sint32_t FwFileClose(FwFile & fp);

/**
 * @brief ファイルを削除
 * @param[in] name  ファイル名
 */
FW_DLL_FUNC sint32_t FwFileDelete(const str_t name);

/**
 * @brief ファイルを読み込む
 * @param[in]  fp           ファイルディスクリプタ
 * @param[in]  dst          読み込み先バッファ
 * @param[in]  toReadSize   読み込みサイズ
 * @param[out] readSize     読み込まれたサイズを格納する変数へのポインタ
 */
FW_DLL_FUNC sint32_t FwFileRead(FwFile & fp, void * dst, const uint64_t toReadSize, uint64_t * readSize);

/**
 * @brief ファイルへ書き込む
 * @param[in] fp          ファイルディスクリプタ
 * @param[in] src         書き込み元バッファ
 * @param[in] toWriteSize 書き込みサイズ
 * @param[in] writeSize   書き込まれたサイズを格納する変数へのポインタ
 */
FW_DLL_FUNC sint32_t FwFileWrite(FwFile & fp, const void * src, const uint64_t toWriteSize, uint64_t * writeSize);

/**
 * @brief ファイルのサイズを取得する
 * @param[in]  name     ファイル名
 * @param[out] length   byte単位の長さ
 */
FW_DLL_FUNC sint32_t FwFileGetLengthByName(const str_t name, uint64_t * length);

/**
 * @brief ファイルのサイズを取得する
 * @param[in]  fp       ファイルディスクリプタ
 * @param[out] length   byte単位の長さ
 */
FW_DLL_FUNC sint32_t FwFileGetLength(FwFile & fp, uint64_t * length);

/**
 * @brief ファイルが存在するか調べる
 * @param[in] relativePath 相対パス
 * @param[in] fileName     ファイル名
 */
FW_DLL_FUNC bool FwFileIsExist(const str_t name);

/**
 * @brief ファイルポインタを移動する
 * @param[in] fp     ファイルディスクリプタ
 * @param[in] offset 基準位置からのオフセット
 * @param[in] origin 基準位置
 */
FW_DLL_FUNC sint32_t FwFileSeek(FwFile & fp, const sint64_t offset, const FwSeekOrigin origin);

/**
 * @brief キャッシュされたデータをフラッシュする
 * @param[in] fp ファイルディスクリプタ
 */
FW_DLL_FUNC sint32_t FwFileFlushBuffer(FwFile & fp);


/**
 * @class FwFile
 * @brief ファイルディスクリプタ
 */
class FwFile {
public:
#if FW_FILE == FW_FILE_WIN32
    HANDLE      nativeHandle;
#elif FW_FILE == FW_FILE_LIBC
    FILE *      nativeHandle;
#endif
    sint32_t    options;

   /**
    * @brief 標準入力デバイスを取得
    */
    static FW_INLINE FwFile StdInputDevice() {
        FwFile file;
        file.options = kFileOptFlagNoClose;

#if FW_FILE == FW_FILE_WIN32
        file.nativeHandle = ::GetStdHandle(STD_INPUT_HANDLE);
#elif FW_FILE == FW_FILE_LIBC
        file.nativeHandle = stdin;
#endif
        return file;
    }

    /**
     * @brief 標準出力デバイスを取得
     */
    static FW_INLINE FwFile StdOutputDevice() {
        FwFile file;
        file.options = kFileOptFlagNoClose;

#if FW_FILE == FW_FILE_WIN32
        file.nativeHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
#elif FW_FILE == FW_FILE_LIBC
        file.nativeHandle = stdout;
#endif
        return file;
    }

    /**
     * @brief 標準エラーデバイスを取得
     */
    static FW_INLINE FwFile StdErrorDevice() {
        FwFile file;
        file.options = kFileOptFlagNoClose;

#if FW_FILE == FW_FILE_WIN32
        file.nativeHandle = ::GetStdHandle(STD_ERROR_HANDLE);
#elif FW_FILE == FW_FILE_LIBC
        file.nativeHandle = stderr;
#endif
        return file;
    }
};

/**
 * @class FwFileHolder
 */
class FwFileHolder {
public:
    FW_INLINE FwFile & Get() {
        return fp_;
    }

    FW_INLINE const FwFile & Get() const {
        return fp_;
    }
    
    FwFileHolder(FwFile & fp)
    : fp_(fp) {
    }

    ~FwFileHolder() {
        FwFileClose(fp_);
    }

private:
    FwFile fp_;
};

END_NAMESPACE_FW

#endif  // FW_FILE_H_
