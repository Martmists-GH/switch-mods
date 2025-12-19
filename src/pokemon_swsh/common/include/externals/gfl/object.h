#pragma once
#include <checks.hpp>
#include <nn/os.h>
#include "externals/util.h"

// TODO: A lot of these are wild guesses and likely wrong, and need a lot of cleanup
namespace gfl {
    struct SizedHeap : ExternalType<SizedHeap> {
        struct vtable {
            void* unk1;
            void* unk2;
            void*(*allocate)(gfl::SizedHeap*, int size, int align);
            void (*deallocate)(gfl::SizedHeap*, void* ptr, int size, int align);
            void* unk5;
            void* unk6;
            void* unk7;
            void* unk8;
            void* unk9;
            void* unk10;
            void* unk11;
            void* unk12;
            void* unk13;
            void* unk14;
            void* unk15;
            void* unk16;
            void* unk17;
            void* unk18;
            void* unk19;
            void* unk20;
            void* unk21;
            void* unk22;
            void* unk23;
            void* unk24;
            void* unk25;
            void* unk26;
            void* unk27;
            void* unk28;
            void(*unk29)(gfl::SizedHeap*);
            void* unk30;
            void* unk31;
            void* unk32;
        };

        struct fields {
            void* unk1;
            char* label;
            char unk2[0x28];
            uint64_t size;
            char unk3[0x8];
            uint64_t allocSizeCount;
            uint64_t allocMax;
        };

        static SizedHeap::instance* s_globalHeap;

        void* Allocate(int size, int align) { return impl()->vtable->allocate(this, size, align); }
        void Deallocate(void* arg, int size, int align) { impl()->vtable->deallocate(this, arg, size, align); }

        uint64_t AllocSize() { return impl()->fields.allocSizeCount & ((~0ull) >> 38); }
        uint64_t AllocCount() { return impl()->fields.allocSizeCount >> 38; }
    };

    struct HeapManager : ExternalType<HeapManager> {
        struct childHeap {
            gfl::SizedHeap::instance* m_heap;
            gfl::SizedHeap::instance* m_parentHeap;
            void* unk1;
            long unk2;
        };

        struct vtable {
            int32_t(*GetCount)(gfl::HeapManager*);
            gfl::SizedHeap::instance*(*GetHeap)(gfl::HeapManager*, int32_t index);
            void* unk2;
            void* unk3;
        };
        struct fields {
            childHeap heaps[4];
        };

        int32_t GetCount() { return impl()->vtable->GetCount(this); }
        gfl::SizedHeap::instance* GetHeap(int32_t index) { return impl()->vtable->GetHeap(this, index); }

        static gfl::HeapManager::instance* s_managerList;
        static uint32_t s_managerCount;
    };
}
