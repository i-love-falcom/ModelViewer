/**
 * @file fw_types.h
 * @author Kamai Masayoshi
 */
#ifndef FW_TYPES_H_
#define FW_TYPES_H_

#if FW_PLATFORM_WIN32
     // 基本型定義
    typedef signed char             sint8_t;    /**< 符号付8bit整数 */
    typedef signed short            sint16_t;   /**< 符号付16bit整数 */
    typedef signed int              sint32i_t;  /**< 符号付32bit整数 */
    typedef signed long             sint32l_t;  /**< 符号付32bit整数 */
    typedef sint32i_t               sint32_t;   /**< 符号付32bit整数 */
    typedef signed long long        sint64_t;   /**< 符号付64bit整数 */
    typedef unsigned char           uint8_t;    /**< 符号無し8bit整数 */
    typedef unsigned short          uint16_t;   /**< 符号無し16bit整数 */
    typedef unsigned int            uint32_t;   /**< 符号無し32bit整数 */
    typedef unsigned long long      uint64_t;   /**< 符号無し64bit整数 */
    typedef sint16_t                float16_t;  /**< 16bit浮動小数点 */

    // 文字列
    #if FW_UNICODE
        typedef wchar_t             char_t; /**< 文字型 */
        typedef wchar_t *           str_t;  /**< 文字列型 */
    #elif FW_MBCS
        typedef char                char_t; /**< 文字型 */
        typedef char*               str_t;  /**< 文字列型 */
    #else
        typedef char                char_t; /**< 文字型 */
        typedef char *              str_t;  /**< 文字列型 */
    #endif

    #define FW_FLT16_MAX            0x7bff
    #define FW_FLT16_MIN            0x8400
    
    #define FW_FLT32_MAX            3.402823466e+38f
    #define FW_FLT32_MIN            1.175494351e-38f
    #define FW_FLT32_EPSILON        1.192092896e-07f 
    
    #define FW_FLT64_MAX            1.7976931348623158e+308
    #define FW_FLT64_MIN            2.2250738585072014e-308
    #define FW_FLT64_EPSILON        2.2204460492503131e-016 
    
    #define FW_INLINE               inline
    #define FW_FORCE_INLINE         __forceinline
    #define FW_WAIT_INFINITE        INFINITE
    
    #define FW_TLS                  __declspec(thread)
    #define FW_ALIGN(__x)           __declspec(align(__x))
#endif

#define FW_ALIGN4                   FW_ALIGN(4)
#define FW_ALIGN8                   FW_ALIGN(8)
#define FW_ALIGN16                  FW_ALIGN(16)
#define FW_ALIGN32                  FW_ALIGN(32)
#define FW_ALIGN64                  FW_ALIGN(64)
#define FW_ALIGN128                 FW_ALIGN(128)
#define FW_ALIGN256                 FW_ALIGN(256)
#define FW_ALIGN512                 FW_ALIGN(512)
#define FW_ALIGN1K                  FW_ALIGN(1024)
#define FW_ALIGN2K                  FW_ALIGN(2048)
#define FW_ALIGN4K                  FW_ALIGN(4096)
#define FW_ALIGN8K                  FW_ALIGN(8192)
#define FW_ALIGN16K                 FW_ALIGN(16384)
#define FW_ALIGN32K                 FW_ALIGN(32768)
#define FW_ALIGN64K                 FW_ALIGN(65536)

#if FW_PLATFORM_WIN64
    #define FW_PLATFORM_ALIGN       FW_ALIGN8
#elif FW_PLATFORM_WIN32
    #define FW_PLATFORM_ALIGN       FW_ALIGN4
#else
    #define FW_PLATFORM_ALIGN       FW_ALIGN16
#endif

#if FW_COMPILER_VS > 0
    #if (FW_COMPILER_VS >= FW_COMPILER_VS2012)
        #define FW_OVERRIDE         override
        #define FW_FINAL            final
    #else
        #define nullptr             NULL
        #define FW_OVERRIDE
        #define FW_FINAL
    #endif
#elif defined(__cplusplus)
    #if (__cplusplus >= 201103L))
        #define FW_OVERRIDE         override
        #define FW_FINAL            final
    #else
        #define nullptr             NULL
        #define FW_OVERRIDE
        #define FW_FINAL
    #endif
#else
    #define FW_OVERRIDE
    #define FW_FINAL
#endif

#endif // FW_TYPES_H_
