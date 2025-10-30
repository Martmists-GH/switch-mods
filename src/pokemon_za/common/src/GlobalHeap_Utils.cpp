#include <vector>
#include <mutex>
#include <externals/gfl/object.h>
#include <logger/logger.h>
#include <nn/ro.h>

struct HeapData {
    void* ptr;
    size_t size;
    size_t alignment;
};

constexpr size_t MAX_HEAP_TRACKED = 0x1000;
static HeapData m_heapAllocData[MAX_HEAP_TRACKED];
static size_t m_heapAllocCount = 0;

using MallocFn  = void* (*)(size_t);
using FreeFn    = void  (*)(void*);
using CallocFn  = void* (*)(size_t, size_t);
using ReallocFn = void* (*)(void*, size_t);

static MallocFn  s_sysMalloc  = nullptr;
static FreeFn    s_sysFree    = nullptr;
static CallocFn  s_sysCalloc  = nullptr;
static ReallocFn s_sysRealloc = nullptr;

static void EnsureSystemAllocators() {
    if (s_sysMalloc) return;

    nn::ro::LookupSymbol((u64*)&s_sysMalloc, "malloc");
    nn::ro::LookupSymbol((u64*)&s_sysFree, "free");
    nn::ro::LookupSymbol((u64*)&s_sysCalloc, "calloc");
    nn::ro::LookupSymbol((u64*)&s_sysRealloc, "realloc");
}

extern "C" {
void* malloc(size_t size) {
    constexpr size_t defaultAlignment = alignof(std::max_align_t);
    auto heap = gfl::SizedHeap::s_globalHeap;

    if (!heap) {
        EnsureSystemAllocators();
        return s_sysMalloc ? s_sysMalloc(size) : nullptr;
    }

    return aligned_alloc(defaultAlignment, size);
}

void* aligned_alloc(size_t alignment, size_t size) {
    auto heap = gfl::SizedHeap::s_globalHeap;
    if (!heap) {
        EnsureSystemAllocators();
        if (s_sysMalloc) {
            void* ptr = nullptr;
            size_t mask = alignment - 1;
            void* raw = s_sysMalloc(size + alignment);
            if (raw) {
                uintptr_t aligned = (reinterpret_cast<uintptr_t>(raw) + mask) & ~mask;
                ptr = reinterpret_cast<void*>(aligned);
                return ptr;
            }
        }
        return nullptr;
    }

    void* ptr = heap->Allocate(size, alignment);
    if (!ptr) return nullptr;

    // std::lock_guard lock(m_heapMutex);
    if (m_heapAllocCount < MAX_HEAP_TRACKED) {
        m_heapAllocData[m_heapAllocCount++] = { ptr, size, alignment };
    }
    return ptr;
}

void free(void* ptr) {
    if (!ptr) return;
    auto heap = gfl::SizedHeap::s_globalHeap;

    if (!heap) {
        EnsureSystemAllocators();
        if (s_sysFree) s_sysFree(ptr);
        return;
    }

    for (size_t i = 0; i < m_heapAllocCount; ++i) {
        if (m_heapAllocData[i].ptr == ptr) {
            heap->Deallocate(ptr, m_heapAllocData[i].size, m_heapAllocData[i].alignment);
            m_heapAllocData[i] = m_heapAllocData[--m_heapAllocCount];
            return;
        }
    }

    EnsureSystemAllocators();
    if (s_sysFree) s_sysFree(ptr);
}

void* calloc(size_t num, size_t size) {
    auto heap = gfl::SizedHeap::s_globalHeap;

    if (!heap) {
        EnsureSystemAllocators();
        return s_sysCalloc ? s_sysCalloc(num, size) : nullptr;
    }

    size_t totalSize = num * size;
    void* ptr = malloc(totalSize);
    if (ptr) std::memset(ptr, 0, totalSize);
    return ptr;
}

void* realloc(void* ptr, size_t newSize) {
    auto heap = gfl::SizedHeap::s_globalHeap;

    if (!heap) {
        EnsureSystemAllocators();
        return s_sysRealloc ? s_sysRealloc(ptr, newSize) : nullptr;
    }

    if (!ptr) return malloc(newSize);
    if (newSize == 0) {
        free(ptr);
        return nullptr;
    }

    for (size_t i = 0; i < m_heapAllocCount; ++i) {
        if (m_heapAllocData[i].ptr == ptr) {
            void* newPtr = heap->Allocate(newSize, m_heapAllocData[i].alignment);
            if (!newPtr) return nullptr;

            std::memcpy(newPtr, ptr, std::min(m_heapAllocData[i].size, newSize));
            heap->Deallocate(ptr, m_heapAllocData[i].size, m_heapAllocData[i].alignment);
            m_heapAllocData[i].ptr = newPtr;
            m_heapAllocData[i].size = newSize;
            return newPtr;
        }
    }

    EnsureSystemAllocators();
    if (s_sysFree) s_sysFree(ptr);
    return nullptr;
}

void* _Znwm(size_t size) {
    return malloc(size);
}

void* _Znam(size_t size) {
    return malloc(size);
}

void _ZdlPv(void* ptr) {
    free(ptr);
}

void _ZdlPvm(void* ptr) {
    free(ptr);
}

void _ZdaPv(void* ptr) {
    free(ptr);
}

}

std::pair<size_t, size_t> GetGlobalHeapState() {
    return {m_heapAllocCount, MAX_HEAP_TRACKED};
}
