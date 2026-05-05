#pragma once
#include <type_traits>
#include <atomic>
#include "allocator.h"

namespace mem {
    struct MemoryObject {
        Allocator* m_allocator = nullptr;
        std::atomic_uint32_t m_refcount = 0;
        uint32_t m_size = 0;
        uint16_t m_align = 1;
        bool m_invalid : 1 = false;

        virtual ~MemoryObject() = default;

        virtual void incrementReference();
        virtual void decrementReference();
        virtual void onZeroReference();

        void* operator new(size_t size, Allocator* allocator, size_t align = 8);
    };

    template <typename T>
    struct MemoryObjectWrapper : MemoryObject {
        T value;

        T* operator->() const {
            return &value;
        }

        T& operator*() const {
            return value;
        }
    };

    template <typename T>
    class Reference {
        static_assert(std::is_base_of_v<MemoryObject, T>);

        T* ptr;
    public:
        Reference(T* ptr) : ptr(ptr) {
            ptr->incref();
        }
        ~Reference() {
            ptr->decref();
        }

        T* operator->() const {
            return ptr;
        }

        T& operator*() const {
            return *ptr;
        }
    };
}
