/**
 * @file fw_define.h
 * @author Kamai Masayoshi
 */
#ifndef FW_DEFINE_H_
#define FW_DEFINE_H_

//---------------------------------------------
// defined platform
//---------------------------------------------
#if defined(WIN32) || defined(WIN64) || defined(__WIN32__)
    #define FW_PLATFORM_WIN32           (1)
    #if defined(WIN64)
        #define FW_PLATFORM_WIN64       (1)
    #endif
    #if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
        #define FW_PLATFORM_WINSTORE    (1)
    #endif

    #if defined(_M_AMD64) || defined(_M_X64)
        #define FW_ARCH_X64             (1)
    #elif defined(_M_IX86)
        #define FW_ARCH_X86             (1)
    #elif defined(_M_ARM)
        #define FW_ARCH_ARM             (1)
    #else
        #error Unsupported architecture
    #endif

    #if defined(_M_ARM_FP)
        #if (30 <= _M_ARM_FP) && (_M_ARM_FP <= 39)
            #define FW_ARCH_ARM_VFPV3   (1)
        #elif (40 <= _M_ARM_FP) && (_M_ARM_FP <= 49)
            #define FW_ARCH_ARM_VFPV4   (1)
        #endif
    #endif

    #if FW_ARCH_X64 || FW_ARCH_X86
        #define FW_LITTLE_ENDIAN        (1)
    #elif FW_ARCH_ARM
        #define FW_BIG_ENDIAN           (1)
    #endif
#elif defined(__APPLE__) || defined(__MACH__)
    #include "TargetConditionals.h"
    #define FW_PLATFORM_MAC             (1)
    
    #if defined(TARGET_CPU_PPC)
        #define FW_ARCH_PPC             (1)
    #elif defined(TARGET_CPU_PPC64)
        #define FW_ARCH_PPC64           (1)
    #elif defined(TARGET_CPU_68K)
        #define FW_ARCH_68K             (1)
    #elif defined(TARGET_CPU_X86)
        #define FW_ARCH_X86             (1)
    #elif defined(TARGET_CPU_X86_64)
        #define FW_ARCH_X64             (1)
    #elif defined(TARGET_CPU_ARM)
        #define FW_ARCH_ARM             (1)
    #elif defined(TARGET_CPU_MIPS)
        #define FW_ARCH_MIPS            (1)
    #elif defined(TARGET_CPU_SPARC)
        #define FW_ARCH_SPARC           (1)
    #elif defined(TARGET_CPU_ALPHA)
        #define FW_ARCH_ALPHA           (1)
    #else
        #error Unsupported architecture
    #endif

    #if TARGET_RT_LITTLE_ENDIAN
        #define FW_LITTLE_ENDIAN        (1)
    #elif TARGET_RT_BIG_ENDIAN
        #define FW_BIG_ENDIAN           (1)
    #endif
#elif defined(__linux__)
#else
    #error Unsupported platform
#endif


//---------------------------------------------
// defined build type
//---------------------------------------------
#if defined(_DEBUG)
    #define FW_DEBUG                    (1)
#else
    #define FW_RELEASE                  (1)
#endif

#if defined(NDEBUG)
    #define FW_NDEBUG                   (1)
#endif

#if defined(USE_STATIC_LIB)
    #define FW_STATIC_LIB               (1)
#else
    #define FW_DYNAMIC_LIB               (1)
    #if defined(EXPORT_DYNCMIC_LIB)
        #define FW_EXPORT_DYNAMIC_LIB   (1)
    #else
        #define FW_IMPORT_DYNAMIC_LIB   (1)
    #endif
#endif

//---------------------------------------------
// defined internal character set
//---------------------------------------------
#if defined(_UNICODE)
    #define FW_UNICODE                  (1)
#elif defined(_MBCS)
    #define FW_MBCS                     (1)
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
    #define FW_COMPILER_MSC             _MSC_VER
#elif defined(__GNUC__)
    #define FW_COMPILER_GCC             __GNUC__
#elif defined(__ICL)
    #define FW_COMPILER_INTEL           __ICL
#elif defined(__clang__)
    #define FW_COMPILER_CLANG           __GNUC__
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
#if defined(FW_COMPILER_MSC) || defined(FW_COMPILER_INTEL)
    #define FW_IMPORT                   __declspec(dllimport)
    #define FW_EXPORT                   __declspec(dllexport)
    #define FW_IMPORT_FUNC              extern "C" __declspec(dllimport)
    #define FW_EXPORT_FUNC              extern "C" __declspec(dllexport)
#elif defined(FW_COMPILER_GCC)
    #define FW_IMPORT                   __attribute__((visibility("default")))
    #define FW_EXPORT                   __attribute__((visibility("default")))
    #define FW_IMPORT_FUNC              __attribute__((visibility("default")))
    #define FW_EXPORT_FUNC              __attribute__((visibility("default")))
#else
    #define FW_IMPORT
    #define FW_EXPORT
    #define FW_IMPORT_FUNC
    #define FW_EXPORT_FUNC
#endif

#if defined(FW_STATIC_LIB)
    #define FW_DLL
    #define FW_DLL_FUNC                 extern
#elif defined(FW_DYNAMIC_LIB)
    #if defined(FW_EXPORT_DYNAMIC_LIB)
        #define FW_DLL                  FW_EXPORT
        #define FW_DLL_FUNC             FW_EXPORT_FUNC
    #else
        #define FW_DLL                  FW_IMPORT
        #define FW_DLL_FUNC             FW_IMPORT_FUNC
    #endif
#else
    #define FW_DLL                      FW_IMPORT
    #define FW_DLL_FUNC                 FW_IMPORT_FUNC
#endif


//---------------------------------------------
// namespace
//---------------------------------------------
#ifdef FW_UNUSE_NAMESPACE
    #define BEGIN_NAMESPACE_FW
    #define END_NAMESPACE_FW
    #define NAMESPACE_FW
    #define USING_NAMESPACE_FW
    
    #define BEGIN_NAMESPACE_NONAME
    #define END_NAMESPACE_NONAME
#else
    #define BEGIN_NAMESPACE_FW          namespace fw {
    #define END_NAMESPACE_FW            }   // namespace "fw"
    #define NAMESPACE_FW                fw::
    #define USING_NAMESPACE_FW          using namespace fw;

    #define BEGIN_NAMESPACE_NONAME      namespace {
    #define END_NAMESPACE_NONAME        }   // namespace ""
#endif

//---------------------------------------------
// macro
//---------------------------------------------
#define FW_ARRAY_SIZEOF(__x)            (sizeof(__x) / sizeof((__x)[0]))

#define FW_SAFE_ADDREF(__x)             if ((__x) != nullptr) {(__x)->AddRef();}
#define FW_SAFE_RELEASE(__x)            if ((__x) != nullptr) {(__x)->Release(); (__x) = nullptr;}

template<typename T>
constexpr T FwBitShift(size_t shift) {
    return static_cast<T>(1) << shift;
}

#define FW_BIT32(__x)                   FwBitShift<uint32_t>(static_cast<size_t>(__x))
#define FW_BIT64(__x)                   FwBitShift<uint64_t>(static_cast<size_t>(__x))

#define FW_DEFINE_ENUM_BITFLAG(T)                                                       \
    constexpr T operator|(const T lhs, const T rhs) {                                   \
        using U = typename std::underlying_type<T>::type;                               \
        return static_cast<T>(static_cast<U>(lhs) | static_cast<U>(rhs));               \
    }                                                                                   \
                                                                                        \
    constexpr T operator|(const T lhs, const std::underlying_type<T>::type rhs) {       \
        using U = typename std::underlying_type<T>::type;                               \
        return static_cast<T>(static_cast<U>(lhs) | rhs);                               \
    }                                                                                   \
                                                                                        \
    constexpr T operator|(const std::underlying_type<T>::type lhs, const T rhs) {       \
        using U = typename std::underlying_type<T>::type;                               \
        return static_cast<T>(lhs | static_cast<U>(rhs));                               \
    }                                                                                   \
                                                                                        \
    constexpr T operator&(const T lhs, const T rhs) {                                   \
        using U = typename std::underlying_type<T>::type;                               \
        return static_cast<T>(static_cast<U>(lhs) & static_cast<U>(rhs));               \
    }                                                                                   \
                                                                                        \
    constexpr T operator&(const T lhs, const std::underlying_type<T>::type rhs) {       \
        using U = typename std::underlying_type<T>::type;                               \
        return static_cast<T>(static_cast<U>(lhs) & rhs);                               \
    }                                                                                   \
                                                                                        \
    constexpr T operator&(const std::underlying_type<T>::type lhs, const T rhs) {       \
        using U = typename std::underlying_type<T>::type;                               \
        return static_cast<T>(lhs & static_cast<U>(rhs));                               \
    }                                                                                   \
                                                                                        \
    constexpr T operator^(const T lhs, const T rhs) {                                   \
        using U = typename std::underlying_type<T>::type;                               \
        return static_cast<T>(static_cast<U>(lhs) ^ static_cast<U>(rhs));               \
    }                                                                                   \
                                                                                        \
    constexpr T operator^(const T lhs, const std::underlying_type<T>::type rhs) {       \
        using U = typename std::underlying_type<T>::type;                               \
        return static_cast<T>(static_cast<U>(lhs) ^ rhs);                               \
    }                                                                                   \
                                                                                        \
    constexpr T operator^(const std::underlying_type<T>::type lhs, const T rhs) {       \
        using U = typename std::underlying_type<T>::type;                               \
        return static_cast<T>(lhs ^ static_cast<U>(rhs));                               \
    }                                                                                   \
                                                                                        \
    constexpr T operator~(const T val) {                                                \
        using U = typename std::underlying_type<T>::type;                               \
        return static_cast<T>(~static_cast<U>(val));                                    \
    }                                                                                   \
                                                                                        \
    inline T& operator|=(T& lhs, const T& rhs) {                                        \
        using U = typename std::underlying_type<T>::type;                               \
        return lhs = static_cast<T>(static_cast<U>(lhs) | static_cast<U>(rhs));         \
    }                                                                                   \
                                                                                        \
    inline T& operator&=(T& lhs, const T& rhs) {                                        \
        using U = typename std::underlying_type<T>::type;                               \
        return lhs = static_cast<T>(static_cast<U>(lhs) & static_cast<U>(rhs));         \
    }                                                                                   \
                                                                                        \
    inline T& operator^=(T& lhs, const T& rhs) {                                        \
        using U = typename std::underlying_type<T>::type;                               \
        return lhs = static_cast<T>(static_cast<U>(lhs) ^ static_cast<U>(rhs));         \
    }                                                                                   \



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
#if defined(FW_DEBUG)
    T p = dynamic_cast<T>(ptr);
    assert(p != nullptr);
    return p;
#else
    return static_cast<T>(ptr);
#endif
}

template<typename T>
constexpr T Min(T a, T b) {
    return a < b ? a : b;
}

template<typename T>
constexpr T Max(T a, T b) {
    return a > b ? a : b;
}

template<typename T>
constexpr T Min3(T a, T b, T c) {
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

template<typename T>
constexpr T Max3(T a, T b, T c) {
    return a > b ? (a > c ? a : c) : (b > c ? b : c);
}

template<typename T>
constexpr T Clamp(T v, T min, T max) {
    return Min<T>(Max<T>(v, min), max);
}

template<typename T, typename U>
constexpr T RoundUp(T ref, const U align) {
    //    const T a = static_cast<const T>(align - 1);
    //    return (ref + a) & ~a;
    return (ref + static_cast<const T>(align - 1)) / static_cast<const T>(align) * static_cast<const T>(align);
}

template<typename T, typename U>
constexpr T RoundDown(T ref, const U align) {
    //    const T a = static_cast<const T>(align - 1);
    //    return ref & ~a;
    return ref / static_cast<const T>(align) * static_cast<const T>(align);
}

template<typename T>
bool IsPowerOfTwo(T number) {
    return ((number != 0) && ((number & (number - 1)) == 0));
}

#endif // FW_DEFINE_H_
