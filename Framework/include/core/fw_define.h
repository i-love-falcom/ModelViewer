/**
 * @file fw_define.h
 * @author Kamai Masayoshi
 */
#ifndef FW_DEFINE_H_
#define FW_DEFINE_H_

//---------------------------------------------
// defined platform
//---------------------------------------------
#if defined(WIN64)
    #define FW_PLATFORM_WIN32           (1)
    #define FW_PLATFORM_WIN64           (1)
    #define FW_PLATFORM_UNKNOWN         (0)
#elif defined(WIN32)
    #define FW_PLATFORM_UNKNOWN         (0)
    #define FW_PLATFORM_WIN32           (1)
    #define FW_PLATFORM_WIN64           (0)
#elif defined(WINRT)
    #define FW_PLATFORM_UNKNOWN         (0)
    #define FW_PLATFORM_WIN32           (0)
    #define FW_PLATFORM_WIN64           (0)
#else
    #define FW_PLATFORM_WIN32           (0)
    #define FW_PLATFORM_WIN64           (0)
    #define FW_PLATFORM_UNKNOWN         (1)
#endif

//---------------------------------------------
// defined cpu architecture
//---------------------------------------------
#if defined(_M_AMD64) || defined(_M_X64)
    #define FW_ARCH_X64                 (1)
#else
    #define FW_ARCH_X64                 (0)
#endif

#if defined(_M_IX86)
    #define FW_ARCH_X86                 (1)
#else
    #define FW_ARCH_X86                 (0)
#endif

#if defined(_M_ARM_FP)
#define FW_ARCH_ARM                 (1)
    #if (30 <= _M_ARM_FP) && (_M_ARM_FP <= 39)
        #define FW_ARCH_ARM_VFPV3       (1)
        #define FW_ARCH_ARM_VFPV4       (0)
    #elif (40 <= _M_ARM_FP) && (_M_ARM_FP <= 49)
        #define FW_ARCH_ARM_VFPV3       (0)
        #define FW_ARCH_ARM_VFPV4       (1)
    #else
        #define FW_ARCH_ARM_VFPV3       (0)
        #define FW_ARCH_ARM_VFPV4       (0)
    #endif
#else
    #define FW_ARCH_ARM                 (0)
    #define FW_ARCH_ARM_VFPV3           (0)
    #define FW_ARCH_ARM_VFPV4           (0)
#endif

#if !FW_ARCH_X86 && !FW_ARCH_X64 && !FW_ARCH_ARM
    #define CORE_ARCH_UNKNOWN           (1)
#else
    #define CORE_ARCH_UNKNOWN           (0)
#endif

//---------------------------------------------
// defined endian type
//---------------------------------------------
#if FW_ARCH_X64 || FW_ARCH_X86
    #define FW_BIG_ENDIAN               (0)
    #define FW_LITTLE_ENDIAN            (1)
#elif FW_ARCH_ARM
    #define FW_BIG_ENDIAN               (1)
    #define FW_LITTLE_ENDIAN            (0)
#else
    #define FW_BIG_ENDIAN               (0)
    #define FW_LITTLE_ENDIAN            (1)
#endif


//---------------------------------------------
// defined build type
//---------------------------------------------
#if defined(_DEBUG)
    #define FW_DEBUG                    (1)
#else
    #define FW_DEBUG                    (0)
#endif

#if defined(_NDEBUG)
    #define FW_RELEASE                  (1)
#else
    #define FW_RELEASE                  (0)
#endif

#if defined(STATIC_LIB)
    #define FW_STATIC_LIB               (1)
    #define FW_DLL_BUILD                (0)
#elif defined(DLL_BUILD)
    #define FW_STATIC_LIB               (0)
    #define FW_DLL_BUILD                (1)
#else
    #define FW_STATIC_LIB               (1)
    #define FW_DLL_BUILD                (0)
#endif


//---------------------------------------------
// defined internal character set
//---------------------------------------------
#if defined(_UNICODE)
    #define FW_UNICODE                  (1)
    #define FW_MBCS                     (0)
#elif defined(_MBCS)
    #define FW_UNICODE                  (0)
    #define FW_MBCS                     (1)
#else
    #define FW_UNICODE                  (1)
    #define FW_MBCS                     (0)
#endif

//---------------------------------------------
// defined compiler version
//---------------------------------------------
#define FW_COMPILER_VS2005              (1400)
#define FW_COMPILER_VS2008              (1500)
#define FW_COMPILER_VS2010              (1600)
#define FW_COMPILER_VS2012              (1700)
#define FW_COMPILER_VS2013              (1800)
#define FW_COMPILER_VS2015              (1900)
#define FW_COMPILER_VS2017              (1910)

#if defined(_MSC_VER)
    #define FW_COMPILER_VS              _MSC_VER
    #define FW_COMPILER_GCC             (0)
#elif defined(__GNUC__)
    #define FW_COMPILER_VS              (0)
    #define FW_COMPILER_GCC             __GNUC__
#else
    #define FW_COMPILER_VS              (0)
    #define FW_COMPILER_GCC             (0)
#endif

//---------------------------------------------
// platform alignment size
//---------------------------------------------
#if FW_PLATFORM_WIN64
    #define FW_PLATFORM_ALIGN_SIZE      8
#elif FW_PLATFORM_WIN32
    #define FW_PLATFORM_ALIGN_SIZE      4
#else
    #define FW_PLATFORM_ALIGN_SIZE      16
#endif

//---------------------------------------------
// dll export 
//---------------------------------------------
#if FW_PLATFORM_WIN32
    #if FW_STATIC_LIB
        #define FW_EXTERN               extern
        #define FW_EXPORT
    #elif FW_DLL_BUILD
        #define FW_EXTERN               extern __declspec(dllexport)
        #define FW_EXPORT               __declspec(dllexport)
    #else
        #define FW_EXTERN               extern __declspec(dllimport)
        #define FW_EXPORT               __declspec(dllimport)
    #endif
#endif

//---------------------------------------------
// namespace
//---------------------------------------------
#ifndef FW_UNUSE_NAMESPACE
    #define BEGIN_NAMESPACE_FW
    #define END_NAMESPACE_FW
    #define USING_NAMESPACE_FW
#else
    #define BEGIN_NAMESPACE_FW          namespace fw {
    #define END_NAMESPACE_FW            }
    #define USING_NAMESPACE_FW          using namespace fw;
#endif

//---------------------------------------------
// macro
//---------------------------------------------
#define ARRAY_SIZEOF(__x)               (sizeof(__x) / sizeof((__x)[0]))

#define SAFE_ADDREF(x__)		        if ((x__) != nullptr) {(x__)->AddRef();}
#define SAFE_RELEASE(x__)               if ((x__) != nullptr) {(x__)->Release(); (x__) = nullptr;}

#define SAFE_DELETE(x__)                if ((x__) != nullptr) {delete (x__); (x__) = nullptr;}
#define SAFE_ARRAY_DELETE(x__)          if ((x__) != nullptr) {delete[] (x__); (x__) = nullptr;}

#define BIT32(x__)                      (1UL << (x__))
#define BIT64(x__)                      (1ULL << (x__))

#ifndef __T
    #if FW_UNICODE
        #define __T(__x)                L ## __x
    #else
        #define __T(__x)                __x
    #endif
#endif

#ifndef _T
    #define _T(__x)                     __T(__x)
#endif
#ifndef TEXT
    #define TEXT(__x)                   __T(__x)
#endif

#define __STRING(__x)                   # __x
#define _STRING(__x)                    __STRING(__x)

#define __TSTRING(__x)                  _T(_STRING(__x))
#define _TSTRING(__x)                   __TSTRING(__x)

//---------------------------------------------
// template
//---------------------------------------------
template<typename T, typename U>
T DYNAMIC_CAST(U ptr) {
#if FW_DEBUG
    T p = dynamic_cast<T>(ptr);
    assert(p != nullptr);
    return p;
#else
    return static_cast<T>(ptr);
#endif
}

template<typename T>
T Min(T a, T b) {
    return a < b ? a : b;
}

template<typename T>
T Max(T a, T b) {
    return a > b ? a : b;
}

template<typename T>
T Min3(T a, T b, T c) {
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

template<typename T>
T Max3(T a, T b, T c) {
    return a > b ? (a > c ? a : c) : (b > c ? b : c);
}

template<typename T>
T Clamp(T v, T min, T max) {
    return Min<T>(Max<T>(v, min), max);
}

template<typename T, typename U>
T RoundUp(T ref, const U align) {
    //    const T a = static_cast<const T>(align - 1);
    //    return (ref + a) & ~a;
    return (ref + static_cast<const T>(align - 1)) / static_cast<const T>(align) * static_cast<const T>(align);
}

template<typename T, typename U>
T RoundDown(T ref, const U align) {
    //    const T a = static_cast<const T>(align - 1);
    //    return ref & ~a;
    return ref / static_cast<const T>(align) * static_cast<const T>(align);
}

#endif // FW_DEFINE_H_
