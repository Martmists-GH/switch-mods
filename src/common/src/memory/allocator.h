#pragma once
#include <cstddef>

namespace mem {
    struct MemoryObject;

    class Allocator {
    public:
        virtual ~Allocator() = default;
        virtual MemoryObject* alloc(size_t size, size_t align) = 0;
        virtual void free(MemoryObject* ptr) = 0;
    };

    class DefaultAllocator : public Allocator {
    public:
        MemoryObject* alloc(size_t size, size_t align) override;
        void free(MemoryObject* ptr) override;
    };
}
