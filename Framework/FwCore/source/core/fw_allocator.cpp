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

/**
 * @struct fwMemBlockHeader
 */
struct fwMemBlockHeader {
    size_t      blockSize;
    uint64_t    attribute;
    uint32_t    offsetBytes;
    uint32_t    marker;
};

/**
 * @struct fwMemBlockFooter
 */
struct fwMemBlockFooter {
    uint32_t    marker;
};

// メモリオーバーラン検出用マーカー
static const uint32_t   s_memBlockMarker = 0xdeadbeef;


static void * fwDefaultMallocFunc(size_t size, uint64_t attribute, const char * file, int line) {
    const size_t numAligned = FW_PLATFORM_ALIGN_SIZE;
    const size_t realSize   = size + sizeof(fwMemBlockHeader) + sizeof(fwMemBlockFooter) + numAligned;
    
    // メモリブロック取得
    void * ptr = _malloc_dbg(realSize, _NORMAL_BLOCK, file, line);
    
    // 先頭アドレス算出
    uintptr_t alignedPtr = RoundUp<uintptr_t>(reinterpret_cast<uintptr_t>(ptr) + sizeof(fwMemBlockHeader), numAligned);
    
    // ヘッダ
    fwMemBlockHeader * header = reinterpret_cast<fwMemBlockHeader *>(alignedPtr - sizeof(fwMemBlockHeader));
    header->blockSize   = size;
    header->attribute   = attribute;
    header->offsetBytes = static_cast<uint32_t>(alignedPtr - reinterpret_cast<uintptr_t>(ptr));
    header->marker      = s_memBlockMarker;

    // フッタ
    fwMemBlockFooter * footer = reinterpret_cast<fwMemBlockFooter *>(alignedPtr + size);
    footer->marker = s_memBlockMarker;

    return reinterpret_cast<void *>(alignedPtr);
}

static void fwDefaultFreeFunc(void * ptr) {
    if (ptr == nullptr) {
        return;
    }

    // ヘッダ
    fwMemBlockHeader * header = reinterpret_cast<fwMemBlockHeader *>(reinterpret_cast<uintptr_t>(ptr) - sizeof(fwMemBlockHeader));
    FwAssert(header->marker == s_memBlockMarker);

    // フッタ
    fwMemBlockFooter * footer = reinterpret_cast<fwMemBlockFooter *>(reinterpret_cast<uintptr_t>(ptr) + header->blockSize);
    FwAssert(footer->marker == s_memBlockMarker);
    
    void * origin = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(ptr) - header->offsetBytes);
    return _free_dbg(origin, _NORMAL_BLOCK);
}



static FWMallocFunc s_currentMallocFunc = fwDefaultMallocFunc;
static FWFreeFunc   s_currentFreeFunc   = fwDefaultFreeFunc;

END_NAMESPACE_NONAME


FWMallocFunc GetDefaultMallocFunc() {
    return fwDefaultMallocFunc;
}

FWFreeFunc GetDefaultFreeFunc() {
    return fwDefaultFreeFunc;
}

FWMallocFunc GetMallocFunc() {
    return s_currentMallocFunc;
}

FWFreeFunc GetFreeFunc() {
    return s_currentFreeFunc;
}

void SetMallocFunc(FWMallocFunc func) {
    s_currentMallocFunc = func;
}

void SetFreeFunc(FWFreeFunc func) {
    s_currentFreeFunc = func;
}


//-----------------------------------------------------------
// 内部使用関数
//-----------------------------------------------------------
#undef FwMalloc
#undef FwFree

void * FwMalloc(size_t size, uint64_t attribute) {
    return s_currentMallocFunc(size, attribute, nullptr, 0);
}

void FwFree(void * ptr) {
    s_currentFreeFunc(ptr);
}

void * FwMallocDebug(size_t size, uint64_t attribute, const char * file, int line) {
    return s_currentMallocFunc(size, attribute, file, line);
}

void FwFreeDebug(void * ptr) {
    return s_currentFreeFunc(ptr);
}
//-----------------------------------------------------------








FwAllocator* FwSetAllocator(FwAllocatorTag tag, FwAllocator *allocator) {
    return nullptr;
}

FwAllocator* FwGetAllocator(FwAllocatorTag tag) {
    return nullptr;
}

END_NAMESPACE_FW
