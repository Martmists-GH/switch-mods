#include "allocator.h"
#include "object.h"

#include <cstdlib>

namespace mem {
    MemoryObject* DefaultAllocator::alloc(size_t size, size_t align) {
        auto ptr = static_cast<MemoryObject*>(std::aligned_alloc(align, size));
        if (ptr) {
            ptr->m_allocator = this;
            ptr->m_size = size;
            ptr->m_align = align;
        }
        return ptr;
    }

    void DefaultAllocator::free(MemoryObject* ptr) {
        if (ptr) {
            std::free(ptr);
        }
    }
}
