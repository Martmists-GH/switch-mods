#include <vector>
#include <mutex>
#include <logger/logger.h>
#include <nn/mem.h>
#include <util/common_utils.h>

#include "hk/mem/BssHeap.h"

namespace yyc {
    extern bool gAllocatorReady;
    extern nn::mem::StandardAllocator gAllocator;
}


static bool gSetForceBssHeap;

void onExceptionHandler() {
    gSetForceBssHeap = true;
}

struct HeapData {
    void* ptr;
    bool isAllocator;
};

constexpr size_t MAX_HEAP_TRACKED = 0x8000;
static HeapData m_heapAllocData[MAX_HEAP_TRACKED];
static size_t m_heapAllocCount = 0;

bool canInitImgui() {
    return yyc::gAllocatorReady;
}

extern "C" {
__attribute__((visibility("hidden")))
void* aligned_alloc(size_t alignment, size_t size) {
    bool isUseAllocator = !gSetForceBssHeap && yyc::gAllocatorReady;
    void* ptr = isUseAllocator
                    ? yyc::gAllocator.Allocate(size, alignment)
                    : hk::mem::sMainHeap.allocate(size, alignment);

    if (!ptr) return nullptr;

    if (m_heapAllocCount < MAX_HEAP_TRACKED) {
        m_heapAllocData[m_heapAllocCount++] = { ptr, isUseAllocator };
    } else {
        Logger::log("Exceeded tracked heap!");
    }

    return ptr;
}

__attribute__((visibility("hidden")))
void* malloc(size_t size) {
    constexpr size_t defaultAlignment = alignof(std::max_align_t);

    return aligned_alloc(defaultAlignment, size);
}

__attribute__((visibility("hidden")))
void free(void* ptr) {
    for (size_t i = 0; i < m_heapAllocCount; ++i) {
        if (m_heapAllocData[i].ptr == ptr) {
            m_heapAllocData[i].ptr = nullptr;
            if (m_heapAllocData[i].isAllocator) {
                yyc::gAllocator.Free(ptr);
            } else {
                hk::mem::sMainHeap.free(ptr);
            }
            if (i == m_heapAllocCount - 1) {
                m_heapAllocCount--;
            }
            return;
        }
    }
}

__attribute__((visibility("hidden")))
void* realloc(void* ptr, size_t newSize) {
    if (!ptr) return malloc(newSize);
    for (size_t i = 0; i < m_heapAllocCount; ++i) {
        if (m_heapAllocData[i].ptr == ptr) {
            void* newPtr;
            if (m_heapAllocData[i].isAllocator) {
                newPtr = yyc::gAllocator.Reallocate(ptr, newSize);
            } else {
                newPtr = hk::mem::sMainHeap.reallocate(ptr, newSize);
            }
            if (newPtr == nullptr) return nullptr;
            m_heapAllocData[i].ptr = newPtr;
        }
    }
    return nullptr;
}

}

