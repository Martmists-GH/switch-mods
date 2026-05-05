#include "object.h"

#include <new>

#include "allocator.h"

namespace mem {
    void MemoryObject::incrementReference() {
        if (m_invalid) return;

        m_refcount.fetch_add(1);
    }

    void MemoryObject::decrementReference() {
        if (m_invalid) return;

        if (m_refcount.fetch_sub(1) - 1 == 0) {
            m_invalid = true;
            onZeroReference();
        }
    }

    void MemoryObject::onZeroReference() {}

    void* MemoryObject::operator new(size_t size, Allocator* allocator, size_t align) {
        return allocator->alloc(size, align);
    }
}
