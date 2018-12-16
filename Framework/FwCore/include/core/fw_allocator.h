/**
 * @file fw_allocator.h
 * @author Kamai Masayoshi
 */
#ifndef FW_ALLOCATOR_H_
#define FW_ALLOCATOR_H_

BEGIN_NAMESPACE_FW

/**
 * @enum FwAllocatorTag
 */
enum class FwAllocatorTag : sint32_t {
    Default = 0,
    Max     = 0xff,
};

/**
 * @class FwAllocactor
 */
class FwAllocator {
protected:

};

/**
 * @brief メモリアロケータを登録
 * @param[in] tag       タグ
 * @param[in] allocator メモリアロケータ
 * @return 以前登録されていたメモリアロケータ
 */
FW_DLL_FUNC FwAllocator* FwSetAllocator(FwAllocatorTag tag, FwAllocator *allocator);

/**
 * @brief 登録されているメモリアロケータを取得
 * @param[in] tag       タグ
 * @return 登録されているメモリアロケータ
 */
FW_DLL_FUNC FwAllocator* FwGetAllocator(FwAllocatorTag tag);





typedef void * (*FWMallocFunc)(size_t, uint64_t, const char *, int);
typedef void (*FWFreeFunc)(void *);

/**
 * @brief 規定のメモリ取得関数を取得
 * @return 既定のメモリ取得関数へのポインタ
 */
FW_DLL_FUNC FWMallocFunc GetDefaultMallocFunc();

/**
 * @brief 規定のメモリ解放関数を取得
 * @return 既定のメモリ解放関数へのポインタ
 */
FW_DLL_FUNC FWFreeFunc GetDefaultFreeFunc();

/**
 * @brief セットされているメモリ取得関数を取得
 * @return メモリ取得関数へのポインタ
 */
FW_DLL_FUNC FWMallocFunc GetMallocFunc();

/**
 * @brief セットされているメモリ解放関数を取得
 * @return メモリ解放関数へのポインタ
 */
FW_DLL_FUNC FWFreeFunc GetFreeFunc();

/**
 * @brief メモリ取得関数をセット
 * @param[in] func メモリ取得関数へのポインタ
 */
FW_DLL_FUNC void SetMallocFunc(FWMallocFunc func);

/**
 * @brief メモリ解放関数をセット
 * @param[in] func メモリ解放関数へのポインタ
 */
FW_DLL_FUNC void SetFreeFunc(FWFreeFunc func);


//-----------------------------------------------------------
// 内部使用関数
//-----------------------------------------------------------
static const uint64_t   defaultFwMallocAttribute = 0;

FW_DLL_FUNC void * FwMalloc(size_t size, uint64_t attribute);
FW_DLL_FUNC void FwFree(void * ptr);

FW_DLL_FUNC void * FwMallocDebug(size_t size, uint64_t attribute, const char * file, int line);
FW_DLL_FUNC void FwFreeDebug(void * ptr);

#if defined(FW_DEBUG)
    #define FwMalloc(__size, __attr)    FwMallocDebug(__size, __attr, __FILE__, __LINE__)
    #define FwFree(__ptr)               FwFreeDebug(__ptr)
#endif


template<class T> T* FwNew() {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T();
}

template<class T, class P1> T* FwNew(P1 & p1) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1);
}

template<class T, class P1> T* FwNew(const P1 & p1) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1);
}

template<class T, class P1, class P2> T* FwNew(P1 & p1, P2 & p2) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2);
}

template<class T, class P1, class P2> T* FwNew(const P1 & p1, P2 & p2) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2);
}

template<class T, class P1, class P2> T* FwNew(P1 & p1, const P2 & p2) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2);
}

template<class T, class P1, class P2> T* FwNew(const P1 & p1, const P2 & p2) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2);
}

template<class T, class P1, class P2, class P3> T* FwNew(P1 & p1, P2 & p2, P2 & p3) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3);
}

template<class T, class P1, class P2, class P3> T* FwNew(const P1 & p1, P2 & p2, P2 & p3) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3);
}

template<class T, class P1, class P2, class P3> T* FwNew(P1 & p1, const P2 & p2, P2 & p3) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3);
}

template<class T, class P1, class P2, class P3> T* FwNew(const P1 & p1, const P2 & p2, P2 & p3) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3);
}

template<class T, class P1, class P2, class P3> T* FwNew(P1 & p1, P2 & p2, const P2 & p3) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3);
}

template<class T, class P1, class P2, class P3> T* FwNew(const P1 & p1, P2 & p2, const P2 & p3) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3);
}

template<class T, class P1, class P2, class P3> T* FwNew(P1 & p1, const P2 & p2, const P2 & p3) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3);
}

template<class T, class P1, class P2, class P3> T* FwNew(const P1 & p1, const P2 & p2, const P2 & p3) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3);
}

template<class T, class P1, class P2, class P3, class P4> T* FwNew(P1 & p1, P2 & p2, P3 & p3, P4 & p4) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4);
}

template<class T, class P1, class P2, class P3, class P4> T* FwNew(const P1 & p1, P2 & p2, P3 & p3, P4 & p4) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4);
}

template<class T, class P1, class P2, class P3, class P4> T* FwNew(P1 & p1, const P2 & p2, P3 & p3, P4 & p4) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4);
}

template<class T, class P1, class P2, class P3, class P4> T* FwNew(const P1 & p1, const P2 & p2, P3 & p3, P4 & p4) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4);
}

template<class T, class P1, class P2, class P3, class P4> T* FwNew(P1 & p1, P2 & p2, const P3 & p3, P4 & p4) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4);
}

template<class T, class P1, class P2, class P3, class P4> T* FwNew(const P1 & p1, P2 & p2, const P3 & p3, P4 & p4) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4);
}

template<class T, class P1, class P2, class P3, class P4> T* FwNew(P1 & p1, const P2 & p2, const P3 & p3, P4 & p4) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4);
}

template<class T, class P1, class P2, class P3, class P4> T* FwNew(const P1 & p1, const P2 & p2, const P3 & p3, P4 & p4) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4);
}

template<class T, class P1, class P2, class P3, class P4> T* FwNew(P1 & p1, P2 & p2, P3 & p3, const P4 & p4) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4);
}

template<class T, class P1, class P2, class P3, class P4> T* FwNew(const P1 & p1, P2 & p2, P3 & p3, const P4 & p4) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4);
}

template<class T, class P1, class P2, class P3, class P4> T* FwNew(P1 & p1, const P2 & p2, P3 & p3, const P4 & p4) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4);
}

template<class T, class P1, class P2, class P3, class P4> T* FwNew(const P1 & p1, const P2 & p2, P3 & p3, const P4 & p4) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4);
}

template<class T, class P1, class P2, class P3, class P4> T* FwNew(P1 & p1, P2 & p2, const P3 & p3, const P4 & p4) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4);
}

template<class T, class P1, class P2, class P3, class P4> T* FwNew(const P1 & p1, P2 & p2, const P3 & p3, const P4 & p4) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4);
}

template<class T, class P1, class P2, class P3, class P4> T* FwNew(P1 & p1, const P2 & p2, const P3 & p3, const P4 & p4) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4);
}

template<class T, class P1, class P2, class P3, class P4> T* FwNew(const P1 & p1, const P2 & p2, const P3 & p3, const P4 & p4) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4);
}

template<class T, class P1, class P2, class P3, class P4, class P5> T* FwNew(P1 & p1, P2 & p2, P3 & p3, P4 & p4, P5 & p5) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4, p5);
}

template<class T, class P1, class P2, class P3, class P4, class P5, class P6> T* FwNew(P1 & p1, P2 & p2, P3 & p3, P4 & p4, P5 & p5, P6 & p6) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4, p5, p6);
}

template<class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7> T* FwNew(P1 & p1, P2 & p2, P3 & p3, P4 & p4, P5 & p5, P6 & p6, P7 & p7) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4, p5, p6, p7);
}

template<class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8> T* FwNew(P1 & p1, P2 & p2, P3 & p3, P4 & p4, P5 & p5, P6 & p6, P7 & p7, P8 & p8) {
    void * p = FwMalloc(sizeof(T), defaultFwMallocAttribute);
    return new(p) T(p1, p2, p3, p4, p5, p6, p7, p8);
}

template<class T> void FwDelete(T * ptr) {
    if (ptr != nullptr) {
        ((T *)ptr)->~T();
        FwFree(ptr);
    }
}

template<class T> void FwDelete(const T * ptr) {
    if (ptr != nullptr) {
        ((T *)ptr)->~T();
        FwFree(const_cast<T*>(ptr));
    }
}

template<class T> T* FwArrayNew(const sint32_t n) {
    const size_t reqSize = RoundUp(sizeof(T) * n + sizeof(sint32_t) * 2, FW_PLATFORM_ALIGN_SIZE);
    void * p = FwMalloc(reqSize, defaultFwMallocAttribute);

    void * ptr = RoundUp(reinterpret_cast<uintptr_t>(p) + sizeof(sint32_t) * 2, FW_PLATFORM_ALIGN_SIZE);
    reinterpret_cast<sint32_t *>(ptr)[-1] = n;
    reinterpret_cast<sint32_t *>(ptr)[-2] = (reinterpret_cast<uintptr_t>(ptr) - reinterpret_cast<uintptr_t>(p)) / FW_PLATFORM_ALIGN_SIZE;

    for (sint32_t i = 0; i < n; ++i) {
        new(reinterpret_cast<void *>(reinterpret_cast<T *>(ptr) + i)) T();
    }
    return ptr;
}

template<class T> T* FwArrayDelete(T* ptr) {
    const sint32_t n      = reinterpret_cast<sint32_t *>(ptr)[-1];
    const sint32_t offset = reinterpret_cast<sint32_t *>(ptr)[-2];

    for (sint32_t i = 0; i < n; ++i) {
        (reinterpret_cast<T *>(ptr) + i)->~T();
    }

    void * p = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(ptr) - offset * FW_PLATFORM_ALIGN_SIZE);
    FwFree(p);
}
//-----------------------------------------------------------

END_NAMESPACE_FW

#endif  // FW_ALLOCATOR_H_
