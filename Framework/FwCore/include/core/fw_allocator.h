/**
 * @file fw_allocator.h
 * @author Kamai Masayoshi
 */
#ifndef FW_ALLOCATOR_H_
#define FW_ALLOCATOR_H_

BEGIN_NAMESPACE_FW

static const sint32_t FwMinMemAllocatorTag      = 0;
static const sint32_t FwMaxMemAllocatorTag      = 0x3ff;
static const sint32_t FwDefaultMemAllocatorTag  = FwMinMemAllocatorTag;

/**
 * @class FwMemAllocator
 */
class FwMemAllocator {
public:
    /**
     * @brief 動的なメモリ確保
     */
    virtual void * Alloc(size_t size, size_t alignment, sint32_t tag) = 0;

    /**
     * @brief 動的メモリを再確保
     */
    virtual void * Realloc(void *ptr, size_t size) = 0;

    /**
     * @brief 動的メモリを解放
     */
    virtual void Free(void *ptr) = 0;


protected:
    /**
     * @brief コンストラクタ
     */
    FwMemAllocator() {
    }

    /**
     * @brief デストラクタ
     */
    virtual ~FwMemAllocator() {
    }
};

/**
 * @brief メモリアロケータを登録
 * @param[in] tag       タグ
 * @param[in] allocator メモリアロケータ
 * @return 以前登録されていたメモリアロケータ
 */
FW_DLL_FUNC FwMemAllocator* FwSetMemAllocator(sint32_t tag, FwMemAllocator *allocator);

/**
 * @brief 登録されているメモリアロケータを取得
 * @param[in] tag タグ
 * @return 登録されているメモリアロケータ
 */
FW_DLL_FUNC FwMemAllocator* FwGetMemAllocator(sint32_t tag);



//-----------------------------------------------------------
// 内部使用関数
//-----------------------------------------------------------
FW_DLL_FUNC void * FwMalloc(size_t size, sint32_t tag);
FW_DLL_FUNC void FwFree(void * ptr);

FW_DLL_FUNC void * FwMallocDebug(size_t size, sint32_t tag, const char * file, int line);
FW_DLL_FUNC void FwFreeDebug(void * ptr);

#if defined(FW_DEBUG)
    #define FwMalloc(__size, __tag)     FwMallocDebug(__size, __tag, __FILE__, __LINE__)
    #define FwFree(__ptr)               FwFreeDebug(__ptr)
#endif


template<class T, sint32_t Tag, class... Args>
T* FwAllocatorNew(Args... args) {
    void * p = FwMalloc(sizeof(T), Tag);
    return new(p) T(args...);
}

template<class T,class... Args>
T* FwNew(Args... args) {
    return FwAllocatorNew<T, FwDefaultMemAllocatorTag>(args...);
}

template<class T>
void FwDelete(T *ptr) {
    if (ptr != nullptr) {
        ((T *)ptr)->~T();
        FwFree(ptr);
    }
}

template<class T>
void FwDelete(const T *ptr) {
    if (ptr != nullptr) {
        ((T *)ptr)->~T();
        FwFree(const_cast<T*>(ptr));
    }
}

template<class T, sint32_t Tag, class... Args>
T* FwAllocatorArrayNew(const sint32_t n, Args... args) {
    const size_t reqSize = RoundUp(sizeof(T) * n + sizeof(sint32_t) * 2, FW_PLATFORM_ALIGN_SIZE);
    void * p = FwMalloc(reqSize, Tag);

    void * ptr = RoundUp(reinterpret_cast<uintptr_t>(p) + sizeof(sint32_t) * 2, FW_PLATFORM_ALIGN_SIZE);
    reinterpret_cast<sint32_t *>(ptr)[-1] = n;
    reinterpret_cast<sint32_t *>(ptr)[-2] = (reinterpret_cast<uintptr_t>(ptr) - reinterpret_cast<uintptr_t>(p)) / FW_PLATFORM_ALIGN_SIZE;

    for (sint32_t i = 0; i < n; ++i) {
        new(reinterpret_cast<void *>(reinterpret_cast<T *>(ptr) + i)) T(args...);
    }
    return ptr;
}

template<class T, class... Args>
T* FwArrayNew(const sint32_t n, Args... args) {
    return FwAllocatorArrayNew<T, FwDefaultMemAllocatorTag>(args...);
}

template<class T>
T* FwArrayDelete(T *ptr) {
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
