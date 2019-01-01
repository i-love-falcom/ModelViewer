/**
 * @file fw_allocator.cpp
 * @author Kamai Masayoshi
 */
#include "precompiled.h"
#include "core/fw_allocator.h"

#include <malloc.h>
#include <crtdbg.h>


BEGIN_NAMESPACE_FW
BEGIN_NAMESPACE_NONAME
//-------------------------------------------------------------------------------------------
// FwMemBlock
//-------------------------------------------------------------------------------------------
/**
 * @struct FwMemBlockHeader
 */
struct FwMemBlockHeader {
    size_t      blockSize;      ///< メモリブロックサイズ
    size_t      alignment;      ///< アライメントサイズ
    uint16_t    offsetBytes;    ///< 実アドレスへのオフセット
    uint16_t    tag;            ///< タグ情報
    uint32_t    magic;          ///< オーバーラン識別用マジック
};

/**
 * @struct FwMemBlockFooter
 */
struct FwMemBlockFooter {
    uint32_t    magic;          ///< オーバーラン識別用マジック
};

// メモリオーバーラン検出用マジック
static const sint32_t   s_memBlockMagic = 0xdeadbeef;

/**
 * @brief メモリブロックからタグを取得
 */
sint32_t FwGetTagFromMemBlock(void *ptr) {
    FwMemBlockHeader * header = reinterpret_cast<FwMemBlockHeader *>(reinterpret_cast<uintptr_t>(ptr) - sizeof(FwMemBlockHeader));
    return header->tag;
}


//-------------------------------------------------------------------------------------------
// FwDefaultAllocator
//-------------------------------------------------------------------------------------------
/**
 * @class FwAllocactor
 */
class FwDefaultAllocator : public FwAllocator {
public:
    /**
     * @brief 動的なメモリ確保
     */
    virtual void * Alloc(size_t size, size_t alignment, sint32_t tag) FW_OVERRIDE {
        const size_t numAligned = Max<size_t>(alignment, FW_PLATFORM_ALIGN_SIZE);
        FwAssert(IsPowerOfTwo(numAligned) && (numAligned / sizeof(void *)) == 0);

        const size_t realSize = size + sizeof(FwMemBlockHeader) + sizeof(FwMemBlockFooter) + numAligned;

        // メモリブロック取得
#if defined(_ISOC11_SOURCE)
        void * ptr = aligned_alloc(numAligned, realSize);
#else
        void * ptr = _aligned_malloc(realSize, numAligned);
#endif

        // 先頭アドレス算出
        uintptr_t alignedPtr = RoundUp<uintptr_t>(reinterpret_cast<uintptr_t>(ptr) + sizeof(FwMemBlockHeader), numAligned);

        // ヘッダ
        FwMemBlockHeader * header = reinterpret_cast<FwMemBlockHeader *>(alignedPtr - sizeof(FwMemBlockHeader));
        header->blockSize = size;
        header->alignment = numAligned;
        header->offsetBytes = static_cast<uint32_t>(alignedPtr - reinterpret_cast<uintptr_t>(ptr));
        header->tag = static_cast<uint16_t>(tag);
        header->magic = s_memBlockMagic;

        // フッタ
        FwMemBlockFooter * footer = reinterpret_cast<FwMemBlockFooter *>(alignedPtr + size);
        footer->magic = s_memBlockMagic;

        return reinterpret_cast<void *>(alignedPtr);
    }

    /**
     * @brief 動的メモリを再確保
     */
    virtual void * Realloc(void *ptr, size_t size) FW_OVERRIDE {
        if (ptr == nullptr || size == 0) {
            return nullptr;
        }

        // ヘッダ
        FwMemBlockHeader * header = reinterpret_cast<FwMemBlockHeader *>(reinterpret_cast<uintptr_t>(ptr) - sizeof(FwMemBlockHeader));
        FwAssert(header->magic == s_memBlockMagic);

        // 現在のサイズより小さければ何もしない
        if (size <= header->blockSize) {
            return ptr;
        }

        void * newptr = Alloc(size, header->alignment, header->tag);
        FwAssert(newptr != nullptr);

        // 内容をコピー
        memcpy(newptr, ptr, header->blockSize);

        // 旧ブロックを破棄
        Free(ptr);

        return newptr;
    }

    /**
     * @brief 動的メモリを解放
     */
    virtual void Free(void *ptr) FW_OVERRIDE {
        if (ptr == nullptr) {
            return;
        }

        // ヘッダ
        FwMemBlockHeader * header = reinterpret_cast<FwMemBlockHeader *>(reinterpret_cast<uintptr_t>(ptr) - sizeof(FwMemBlockHeader));
        FwAssert(header->magic == s_memBlockMagic);

        // フッタ
        FwMemBlockFooter * footer = reinterpret_cast<FwMemBlockFooter *>(reinterpret_cast<uintptr_t>(ptr) + header->blockSize);
        FwAssert(footer->magic == s_memBlockMagic);

        void * origin = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(ptr) - header->offsetBytes);
#if defined(_ISOC11_SOURCE)
        free(origin);
#else
        _aligned_free(origin);
#endif
    }

    /**
     * @brief コンストラクタ
     */
    FwDefaultAllocator() {
        ;
    }

    /**
     * @brief デストラクタ
     */
    virtual ~FwDefaultAllocator() {
        ;
    }
};


static FwDefaultAllocator   s_defaultAllocator;
static FwAllocator         *s_allocatorArray[FwMaxAllocatorTag + 1] = { &s_defaultAllocator };

END_NAMESPACE_NONAME


FwAllocator* FwSetAllocator(sint32_t tag, FwAllocator *allocator) {
    FwAssert(FwMinAllocatorTag <= tag && tag <= FwMaxAllocatorTag);
    FwAllocator *oldAllocator = s_allocatorArray[tag];
    s_allocatorArray[tag] = allocator;
    return oldAllocator;
}

FwAllocator* FwGetAllocator(sint32_t tag) {
    FwAssert(FwMinAllocatorTag <= tag && tag <= FwMaxAllocatorTag);
    return s_allocatorArray[tag];
}


//-----------------------------------------------------------
// 内部使用関数
//-----------------------------------------------------------
#undef FwMalloc
#undef FwFree

void * FwMalloc(size_t size, sint32_t tag) {
    auto allocator = FwGetAllocator(tag);
    if (allocator != nullptr) {
        return allocator->Alloc(size, FW_PLATFORM_ALIGN_SIZE, tag);
    }
    return nullptr;
}

void FwFree(void * ptr) {
    if (ptr != nullptr) {
        auto tag = FwGetTagFromMemBlock(ptr);
        auto allocator = FwGetAllocator(tag);
        allocator->Free(ptr);
    }
}

void * FwMallocDebug(size_t size, sint32_t tag, const char * file, int line) {
    //! @todo デバッグ情報収集
    return FwMalloc(size, tag);
}

void FwFreeDebug(void * ptr) {
    //! @todo デバッグ情報収集
    FwFree(ptr);
}
//-----------------------------------------------------------


END_NAMESPACE_FW
