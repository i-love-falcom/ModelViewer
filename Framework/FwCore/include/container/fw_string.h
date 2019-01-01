/**
 * @file fw_string.h
 * @author Kamai Masayoshi
 */
#ifndef FW_STRING_H_
#define FW_STRING_H_

#include <string>
#include <errno.h>

#include "container/fw_container_allocator.h"

BEGIN_NAMESPACE_FW

template<typename _Ty, uint32_t _Tag = FwDefaultAllocatorTag>
class basic_string : public std::basic_string<_Ty, std::char_traits<_Ty>, NAMESPACE_FW allocator<_Ty, _Tag> > {
public:
#if defined(FW_PLATFORM_WIN32)
    /**
     * @brief ロケール設定に従ってwchat_tからcharへ文字列を変換
     * @param[out] dstValue 変換された文字数
     * @param[out] dst      出力バッファ
     * @param[in]  dstSize  出力バッファサイズ（文字数）
     * @param[in]  src      入力バッファ
     * @param[in]  srcLen   変換文字数
     */
    static FW_INLINE sint32_t WCharToChar(size_t * dstValue, char * dst, size_t dstSize, const wchar_t * src, size_t srcLen) {
        errno_t err = wcstombs_s(dstValue, dst, dstSize, src, srcLen);
        return static_cast<sint32_t>(err);
    }

    /**
     * @brief ロケール設定に従ってwchat_tからcharへ文字列を変換
     * @param[out] dstValue 変換された文字数
     * @param[out] dst      出力バッファ
     * @param[in]  dstSize  出力バッファサイズ（文字数）
     * @param[in]  src      入力バッファ
     * @param[in]  srcLen   変換文字数
     */
    static FW_INLINE sint32_t CharToWChar(size_t * dstValue, wchar_t * dst, size_t dstSize, const char * src, size_t srcLen) {
        errno_t err = mbstowcs_s(dstValue, dst, dstSize, src, srcLen);
        return static_cast<sint32_t>(err);
    }

    /**
     * @brief 大文字へ変換
     * @param[out] dst           出力バッファ
     * @param[in]  numOfElements 出力バッファサイズ（文字数）
     * @param[in]  src           入力バッファ
     */
    static FW_INLINE void ToUpper(char * dst, size_t numOfElements, const char * src) {
        const size_t len = Min(numOfElements - 1, Length(src));
        for (size_t i = 0; i < len; ++i) {
            dst[i] = toupper(src[i]);
        }
        dst[len] = '\0';
    }

    /**
     * @brief 大文字へ変換
     * @param[out] dst           出力バッファ
     * @param[in]  numOfElements 出力バッファサイズ（文字数）
     * @param[in]  src           入力バッファ
     */
    static FW_INLINE void ToUpper(unsigned char * dst, size_t numOfElements, const unsigned char * src) {
        const size_t len = Min(numOfElements - 1, Length(src));
        for (size_t i = 0; i < len; ++i) {
            dst[i] = toupper(src[i]);
        }
        dst[len] = '\0';
    }

    /**
     * @brief 大文字へ変換
     * @param[out] dst           出力バッファ
     * @param[in]  numOfElements 出力バッファサイズ（文字数）
     * @param[in]  src           入力バッファ
     */
    static FW_INLINE void ToUpper(wchar_t * dst, size_t numOfElements, const wchar_t * src) {
        const size_t len = Min(numOfElements - 1, Length(src));
        for (size_t i = 0; i < len; ++i) {
            dst[i] = towupper(src[i]);
        }
        dst[len] = L'\0';
    }

    /**
     * @brief 小文字へ変換
     * @param[out] dst           出力バッファ
     * @param[in]  numOfElements 出力バッファサイズ（文字数）
     * @param[in]  src           入力バッファ
     */
    static FW_INLINE void ToLower(char * dst, size_t numOfElements, const char * src) {
        const size_t len = Min(numOfElements - 1, Length(src));
        for (size_t i = 0; i < len; ++i) {
            dst[i] = tolower(src[i]);
        }
        dst[len] = '\0';
    }

    /**
     * @brief 小文字へ変換
     * @param[out] dst           出力バッファ
     * @param[in]  numOfElements 出力バッファサイズ（文字数）
     * @param[in]  src           入力バッファ
     */
    static FW_INLINE void ToLower(unsigned char * dst, size_t numOfElements, const unsigned char * src) {
        const size_t len = Min(numOfElements - 1, Length(src));
        for (size_t i = 0; i < len; ++i) {
            dst[i] = tolower(src[i]);
        }
        dst[len] = '\0';
    }

    /**
     * @brief 小文字へ変換
     * @param[out] dst           出力バッファ
     * @param[in]  numOfElements 出力バッファサイズ（文字数）
     * @param[in]  src           入力バッファ
     */
    static FW_INLINE void ToLower(wchar_t * dst, size_t numOfElements, const wchar_t * src) {
        const size_t len = Min(numOfElements - 1, Length(src));
        for (size_t i = 0; i < len; ++i) {
            dst[i] = towlower(src[i]);
        }
        dst[len] = L'\0';
    }

    /**
     * @brief 文字列の長さを取得
     * @param[in] src
     * @return 文字数
     */
    static FW_INLINE size_t Length(const char * src) {
        return strlen(src);
    }

    /**
     * @brief 文字列の長さを取得
     * @param[in] src
     * @return 文字数
     */
    static FW_INLINE size_t Length(const unsigned char * src) {
        return _mbslen(src);
    }

    /**
     * @brief 文字列の長さを取得
     * @param[in] src
     * @return 文字数
     */
    static FW_INLINE size_t Length(const wchar_t * src) {
        return wcslen(src);
    }

    /**
     * @brief 文字列のコピー
     * @param[out] dst           出力バッファ
     * @param[in]  numOfElements 出力バッファサイズ（文字数）
     * @param[in]  src           入力バッファ
     */
    static FW_INLINE void Copy(char * dst, size_t numOfElements, const char * src) {
        strcpy_s(dst, numOfElements, src);
    }

    /**
     * @brief 文字列のコピー
     * @param[out] dst           出力バッファ
     * @param[in]  numOfElements 出力バッファサイズ（文字数）
     * @param[in]  src           入力バッファ
     */
    static FW_INLINE void Copy(unsigned char * dst, size_t numOfElements, const unsigned char * src) {
        _mbscpy_s(dst, numOfElements, src);
    }

    /**
     * @brief 文字列のコピー
     * @param[in] dst
     * @param[in] numOfElements
     * @param[in] src
     */
    static FW_INLINE void Copy(wchar_t * dst, size_t numOfElements, const wchar_t * src) {
        wcscpy_s(dst, numOfElements, src);
    }

    /**
     * @brief 文字数を指定して文字列をコピー
     * @param[out] dst           出力バッファ
     * @param[in]  numOfElements 出力バッファサイズ（文字数）
     * @param[in]  src           入力バッファ
     * @param[in]  count         コピーする文字数
     */
    static FW_INLINE void CopyN(char * dst, size_t numOfElements, const char * src, size_t count) {
        strncpy_s(dst, numOfElements, src, count);
    }

    /**
     * @brief 文字数を指定して文字列をコピー
     * @param[out] dst           出力バッファ
     * @param[in]  numOfElements 出力バッファサイズ（文字数）
     * @param[in]  src           入力バッファ
     * @param[in]  count         コピーする文字数
     */
    static FW_INLINE void CopyN(unsigned char * dst, size_t numOfElements, const unsigned char * src, size_t count) {
        _mbsncpy_s(dst, numOfElements, src, count);
    }

    /**
     * @brief 文字数を指定して文字列をコピー
     * @param[out] dst           出力バッファ
     * @param[in]  numOfElements 出力バッファサイズ（文字数）
     * @param[in]  src           入力バッファ
     * @param[in]  count         コピーする文字数
     */
    static FW_INLINE void CopyN(wchar_t * dst, size_t numOfElements, const wchar_t * src, size_t count) {
        wcsncpy_s(dst, numOfElements, src, count);
    }

    /**
     * @brief 文字列の連結
     * @param[out] dst           出力バッファ
     * @param[in]  numOfElements 出力バッファサイズ（文字数）
     * @param[in]  src           入力バッファ
     */
    static FW_INLINE void Concat(char * dst, size_t numOfElements, const char * src) {
        strcat_s(dst, numOfElements, src);
    }

    /**
     * @brief 文字列の連結
     * @param[out] dst           出力バッファ
     * @param[in]  numOfElements 出力バッファサイズ（文字数）
     * @param[in]  src           入力バッファ
     */
    static FW_INLINE void Concat(unsigned char * dst, size_t numOfElements, const unsigned char * src) {
        _mbscat_s(dst, numOfElements, src);
    }

    /**
     * @brief 文字列の連結
     * @param[out] dst           出力バッファ
     * @param[in]  numOfElements 出力バッファサイズ（文字数）
     * @param[in]  src           入力バッファ
     */
    static FW_INLINE void Concat(wchar_t * dst, size_t numOfElements, const wchar_t * src) {
        wcscat_s(dst, numOfElements, src);
    }

    /**
     * @brief 文字数を指定して文字列を連結
     * @param[out] dst           追加先バッファ
     * @param[in]  numOfElements 出力バッファサイズ（文字数）
     * @param[in]  src           入力バッファ
     * @param[in]  count         追加する文字数
     */
    static FW_INLINE void ConcatN(char * dst, size_t numOfElements, const char * src, size_t count) {
        strncat_s(dst, numOfElements, src, count);
    }

    /**
     * @brief 文字数を指定して文字列を連結
     * @param[out] dst           追加先バッファ
     * @param[in]  numOfElements 出力バッファサイズ（文字数）
     * @param[in]  src           入力バッファ
     * @param[in]  count         追加する文字数
     */
    static FW_INLINE void ConcatN(unsigned char * dst, size_t numOfElements, const unsigned char * src, size_t count) {
        _mbsncat_s(dst, numOfElements, src, count);
    }

    /**
     * @brief 文字数を指定して文字列を連結
     * @param[out] dst           追加先バッファ
     * @param[in]  numOfElements 出力バッファサイズ（文字数）
     * @param[in]  src           入力バッファ
     * @param[in]  count         追加する文字数
     */
    static FW_INLINE void ConcatN(wchar_t * dst, size_t numOfElements, const wchar_t * src, size_t count) {
        wcsncat_s(dst, numOfElements, src, count);
    }

    /**
     * @brief 文字列の比較
     * @param[in] s1 入力文字列1
     * @param[in] s2 入力文字列2
     * @return 比較結果
     */
    static FW_INLINE sint32_t Cmp(const char * s1, const char * s2) {
        return strcmp(s1, s2);
    }

    /**
     * @brief 文字列の比較
     * @param[in] s1 入力文字列1
     * @param[in] s2 入力文字列2
     * @return 比較結果
     */
    static FW_INLINE sint32_t Cmp(const unsigned char * s1, const unsigned char * s2) {
        return _mbscmp(s1, s2);
    }

    /**
     * @brief 文字列の比較
     * @param[in] s1 入力文字列1
     * @param[in] s2 入力文字列2
     * @return 比較結果
     */
    static FW_INLINE sint32_t Cmp(const wchar_t * s1, const wchar_t * s2) {
        return wcscmp(s1, s2);
    }

    /**
     * @brief 書式付き文字列の出力
     * @param[in] dst           出力バッファ
     * @param[in] numOfElements 出力バッファサイズ（文字数）
     * @param[in] fmt           書式指定
     * @param[in] ap            引数リスト
     * @return 書き込まれた文字数
     */
    static FW_INLINE sint32_t VSPrintf(char * dst, size_t numOfElements, const char * fmt, va_list ap) {
        return vsprintf_s(dst, numOfElements, fmt, ap);
    }

    /**
     * @brief 書式付き文字列の出力
     * @param[in] dst           出力バッファ
     * @param[in] numOfElements 出力バッファサイズ（文字数）
     * @param[in] fmt           書式指定
     * @param[in] ap            引数リスト
     * @return 書き込まれた文字数
     */
    static FW_INLINE sint32_t VSPrintf(wchar_t * dst, size_t numOfElements, const wchar_t * fmt, va_list ap) {
        return vswprintf_s(dst, numOfElements, fmt, ap);
    }
#else
    #error Unsupported platform
#endif

    /**
     * @brief 大文字へ変換
     */
    FW_INLINE void ToUpper() {
        _Ty dst = static_cast<_Ty>(FwMalloc(sizeof(_Ty) * this->length(), _Tag));
        ToUpper(dst, this->length(), this->c_str());
        this->replace(0, this->length(), dst);
        FwFree(dst);
    }

    /**
     * @brief 小文字へ変換
     * @param[in] src
     */
    FW_INLINE void ToLower() {
        _Ty dst = static_cast<_Ty>(FwMalloc(sizeof(_Ty) * this->length(), _Tag));
        ToLower(dst, this->length(), this->c_str());
        this->replace(0, this->length(), dst);
        FwFree(dst);
    }

    /**
     * @brief 文字列の比較
     * @param[in] s2 比較文字列
     * @return 比較結果
     */
    FW_INLINE sint32_t Cmp(const _Ty s2) {
        return Cmp(this->c_str(), s2);
    }

    /**
     * @brief 大文字小文字を考慮せず文字列の比較
     * @param[in] s2 比較文字列
     * @return 比較結果
     */
    FW_INLINE sint32_t ICmp(const _Ty s2) {
        return ICmp(this->c_str(), s2);
    }

   /**
     * @brief 大文字小文字を考慮せず文字列の比較
     * @param[in] s1 入力文字列1
     * @param[in] s2 入力文字列2
     * @return 比較結果
     */
    static FW_INLINE sint32_t ICmp(const char * s1, const char * s2) {
        size_t s1Len = Length(s1) + 1;
        size_t s2Len = Length(s2) + 1;

        char * dstS1 = reinterpret_cast<char *>(FwMalloc(sizeof(char *) * s1Len, _Tag));
        char * dstS2 = reinterpret_cast<char *>(FwMalloc(sizeof(char *) * s2Len, _Tag));

        ToUpper(dstS1, s1Len, s1);
        ToUpper(dstS2, s2Len, s2);

        sint32_t ret = strcmp(dstS1, dstS2);

        FwFree(dstS1);
        FwFree(dstS2);

        return ret;
    }

    /**
     * @brief 大文字小文字を考慮せず文字列の比較
     * @param[in] s1 入力文字列1
     * @param[in] s2 入力文字列2
     * @return 比較結果
     */
    static FW_INLINE sint32_t ICmp(const unsigned char * s1, const unsigned char * s2) {
        size_t s1Len = Length(s1) + 1;
        size_t s2Len = Length(s2) + 1;

        unsigned char * dstS1 = reinterpret_cast<unsigned char *>(FwMalloc(sizeof(unsigned char *) * s1Len, _Tag));
        unsigned char * dstS2 = reinterpret_cast<unsigned char *>(FwMalloc(sizeof(unsigned char *) * s2Len, _Tag));

        ToUpper(dstS1, s1Len, s1);
        ToUpper(dstS2, s2Len, s2);

        sint32_t ret = _mbscmp(dstS1, dstS2);

        FwFree(dstS1);
        FwFree(dstS2);

        return ret;

    }

    /**
     * @brief 大文字小文字を考慮せず文字列の比較
     * @param[in] s1 入力文字列1
     * @param[in] s2 入力文字列2
     * @return 比較結果
     */
    static FW_INLINE sint32_t ICmp(const wchar_t * s1, const wchar_t * s2) {
        size_t s1Len = Length(s1) + 1;
        size_t s2Len = Length(s2) + 1;

        wchar_t * dstS1 = reinterpret_cast<wchar_t *>(FwMalloc(sizeof(wchar_t *) * s1Len, _Tag));
        wchar_t * dstS2 = reinterpret_cast<wchar_t *>(FwMalloc(sizeof(wchar_t *) * s2Len, _Tag));

        ToUpper(dstS1, s1Len, s1);
        ToUpper(dstS2, s2Len, s2);

        sint32_t ret = wcscmp(dstS1, dstS2);

        FwFree(dstS1);
        FwFree(dstS2);

        return ret;
    }

    /**
     * @brief 書式付き文字列の出力
     * @param[in] dst           出力バッファ
     * @param[in] numOfElements 出力バッファサイズ（文字数）
     * @param[in] fmt           書式指定
     * @return 書き込まれた文字数
     */
    static FW_INLINE sint32_t SPrintf(char * dst, size_t numOfElements, const char * fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        sint32_t ret = VSPrintf(dst, numOfElements, fmt, ap);
        va_end(ap);
        return ret;
    }

    /**
     * @brief 書式付き文字列の出力
     * @param[in] dst           出力バッファ
     * @param[in] numOfElements 出力バッファサイズ（文字数）
     * @param[in] fmt           書式指定
     * @return 書き込まれた文字数
     */
    static FW_INLINE sint32_t SPrintf(unsigned char * dst, size_t numOfElements, const unsigned char * fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        sint32_t ret = VSPrintf(dst, numOfElements, fmt, ap);
        va_end(ap);
        return ret;
    }

    /**
     * @brief 書式付き文字列の出力
     * @param[in] dst           出力バッファ
     * @param[in] numOfElements 出力バッファサイズ（文字数）
     * @param[in] fmt           書式指定
     * @return 書き込まれた文字数
     */
    static FW_INLINE sint32_t SPrintf(wchar_t * dst, size_t numOfElements, const wchar_t * fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        sint32_t ret = VSPrintf(dst, numOfElements, fmt, ap);
        va_end(ap);
        return ret;
    }
};


class tstring : public NAMESPACE_FW basic_string<char_t> {};
class string  : public NAMESPACE_FW basic_string<char> {};
class wstring : public NAMESPACE_FW basic_string<wchar_t> {};

END_NAMESPACE_FW

#endif  // FW_STRING_H_
