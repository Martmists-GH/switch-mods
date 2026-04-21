#pragma once
#include <cstdint>

#include "IDisposer.h"
#include "Reflexible.h"

namespace sead {
    struct ListNode {
        sead::ListNode* m_prev;
        sead::ListNode* m_next;
    };

    template <typename T>
    struct OffsetList {
        sead::ListNode m_startEnd;
        int m_size;
        int m_offset;

        T* get(int index) {
            auto current = m_startEnd.m_next;
            for (int c = 0; c < index; c++) {
                current = current->m_next;
            }
            return (T*)((uintptr_t)current - m_offset);
        }
    };

    struct Heap : ExternalType<Heap> {
        struct vtable : sead::IDisposer::vtable {
            sead::Reflexible::vtable as_Reflexible;
            void* destroy;
            void* adjust;
            void*(*tryAlloc)(sead::Heap*, size_t, size_t);
            void(*free)(sead::Heap*, void*);
            void* freeAndGetAllocatableSize;
            void* resizeFront;
            void* resizeBack;
            void*(*tryRealloc)(sead::Heap*, void*, size_t);
            void* freeAll;
            void* getStartAddress;
            void* getEndAddress;
            void* getSize;
            size_t(*getFreeSize)(sead::Heap*);
            void* getUsedSize;
            void* getMaxAllocSize;
            // TODO
        };
        static_assert(offsetof(vtable, tryAlloc) == 0x30);

        struct fields : sead::IDisposer::fields {
            void* m_name;
            void* m_start;
            size_t m_size;
            sead::Heap* m_parent;
            sead::OffsetList<sead::Heap> m_children;
            sead::ListNode m_listNode;
        };

        void* alloc(size_t size, size_t align = alignof(std::max_align_t)) {
            return impl()->vtable->tryAlloc(castTo<sead::Heap>(), size, align);
        }

        void* realloc(void* ptr, size_t size) {
            return impl()->vtable->tryRealloc(castTo<sead::Heap>(), ptr, size);
        }

        void free(void* ptr) {
            return impl()->vtable->free(castTo<sead::Heap>(), ptr);
        }

        size_t getFreeSize() {
            return impl()->vtable->getFreeSize(castTo<sead::Heap>());
        }
    };

    struct ExpHeap : ExternalType<ExpHeap> {
        struct vtable : Heap::vtable {};
        struct fields : Heap::fields {};

        void* alloc(size_t size, size_t align = alignof(std::max_align_t)) {
            return impl()->vtable->tryAlloc(castTo<sead::Heap>(), size, align);
        }

        void* realloc(void* ptr, size_t size) {
            return impl()->vtable->tryRealloc(castTo<sead::Heap>(), ptr, size);
        }

        void free(void* ptr) {
            return impl()->vtable->free(castTo<sead::Heap>(), ptr);
        }

        size_t getFreeSize() {
            return impl()->vtable->getFreeSize(castTo<sead::Heap>());
        }
    };

    struct AllocatorList {
        int m_size;
        int m_capacity;
        ExpHeap** m_items;
    };

    extern AllocatorList gAllocatorList;
}
