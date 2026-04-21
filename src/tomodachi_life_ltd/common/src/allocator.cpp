// #include <new>
//
// #include "nn/mem.h"
// #include "externals/sead/Heap.h"
// #include "logger/logger.h"
//
// extern nn::mem::StandardAllocator gGlobalAllocator;
//
// struct HeapData {
//     void* ptr;
//     sead::Heap::instance* heap;
// };
//
// constexpr size_t MAX_HEAP_TRACKED = 0x8000;
// static HeapData m_heapAllocData[MAX_HEAP_TRACKED];
// static size_t m_heapAllocCount = 0;
//
// __attribute__((weak))
// bool canInitImgui() {
//     return sead::gAllocatorList.m_items[0]->impl()->fields.m_children.m_size >= 6;
// }
//
// extern "C" {
// __attribute__((visibility("hidden")))
// void* aligned_alloc(size_t alignment, size_t size) {
//     auto heap = sead::gAllocatorList.m_items[0]->castTo<sead::Heap>();
//
//     if (heap->fields.m_children.m_size >= 6) {
//         heap = heap->fields.m_children.get(5)->impl();  // sead::TaskMgr
//         heap = heap->fields.m_children.get(0)->impl();  // RootTask
//         heap = heap->fields.m_children.get(5)->impl();  // ResourceMgr
//         heap = heap->fields.m_children.get(1)->impl();  // ForResourceL
//     } else {
//         auto ptr = gGlobalAllocator.Allocate(size, alignment);
//         if (!ptr) return nullptr;
//         if (m_heapAllocCount < MAX_HEAP_TRACKED) {
//             m_heapAllocData[m_heapAllocCount++] = { ptr, nullptr };
//         }
//         return ptr;
//     }
//
//     auto ptr = heap->alloc(size, alignment);
//     if (!ptr) return nullptr;
//
//     if (m_heapAllocCount < MAX_HEAP_TRACKED) {
//         m_heapAllocData[m_heapAllocCount++] = { ptr, heap };
//     }
//
//     return ptr;
// }
//
// __attribute__((visibility("hidden")))
// void* malloc(size_t size) {
//     constexpr size_t defaultAlignment = alignof(std::max_align_t);
//
//     return aligned_alloc(defaultAlignment, size);
// }
//
// __attribute__((visibility("hidden")))
// void free(void* ptr) {
//     for (size_t i = 0; i < m_heapAllocCount; ++i) {
//         if (m_heapAllocData[i].ptr == ptr) {
//             m_heapAllocData[i].ptr = nullptr;
//             auto heap = m_heapAllocData[i].heap;
//             if (heap == nullptr) {
//                 gGlobalAllocator.Free(ptr);
//             } else {
//                 heap->free(ptr);
//             }
//             if (i == m_heapAllocCount - 1) {
//                 m_heapAllocCount--;
//             }
//             return;
//         }
//     }
// }
//
// __attribute__((visibility("hidden")))
// void* calloc(size_t num, size_t size) {
//     return aligned_alloc(size, num * size);
// }
//
// __attribute__((visibility("hidden")))
// void* realloc(void* ptr, size_t newSize) {
//     for (size_t i = 0; i < m_heapAllocCount; ++i) {
//         if (m_heapAllocData[i].ptr == ptr) {
//             auto heap = m_heapAllocData[i].heap;
//             if (heap == nullptr) {
//                 return gGlobalAllocator.Reallocate(ptr, newSize);
//             } else {
//                 return heap->realloc(ptr, newSize);
//             }
//         }
//     }
//     return nullptr;
// }
//
// __attribute__((visibility("hidden")))
// void* _Znwm(size_t size) {
//     return malloc(size);
// }
//
// __attribute__((visibility("hidden")))
// void* _ZnwmSt11align_val_t(size_t size, uint64_t align) {
//     return aligned_alloc(align, size);
// }
//
// __attribute__((visibility("hidden")))
// void* _Znam(size_t size) {
//     return malloc(size);
// }
//
// __attribute__((visibility("hidden")))
// void* _ZnamSt11align_val_t(size_t size, uint64_t align) {
//     return aligned_alloc(align, size);
// }
//
// __attribute__((visibility("hidden")))
// void _ZdlPv(void* ptr) {
//     free(ptr);
// }
//
// __attribute__((visibility("hidden")))
// void _ZdlPvSt11align_val_t(void* ptr, uint64_t align) {
//     free(ptr);
// }
//
// __attribute__((visibility("hidden")))
// void _ZdlPvm(void* ptr) {
//     free(ptr);
// }
//
// __attribute__((visibility("hidden")))
// void _ZdlPvmSt11align_val_t(void* ptr, uint64_t size, uint64_t align) {
//     free(ptr);
// }
//
// __attribute__((visibility("hidden")))
// void _ZdaPv(void* ptr) {
//     free(ptr);
// }
//
// }
