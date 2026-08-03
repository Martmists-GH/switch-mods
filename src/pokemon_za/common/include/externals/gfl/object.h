#pragma once
#include <checks.hpp>
#include <nn/os.h>
#include "externals/util.h"

// TODO: A lot of these are wild guesses and likely wrong, and need a lot of cleanup
namespace gfl {
    struct SizedHeap;

    struct IReferenceObject : ExternalInterface<IReferenceObject> {
        struct vtable {
            void* unk1;  // Destructor?
            void* unk2;  // Destructor?
            void* IncrementReference;
            void* DecrementReference;
        };
    };

    struct IObjectHandler : ExternalInterface<IObjectHandler> {
        struct vtable : gfl::IReferenceObject::vtable {
            void* GetHandle;
            void* GetWeakHandle;
            void* unk7;  // GetReflection?
            void* unk8;
        };
    };

    struct Reflection {
        gfl::Reflection* m_parent;
    };

    struct IReflectionObject : ExternalInterface<IReflectionObject> {
        struct vtable {
            gfl::Reflection*(*GetReflection)(gfl::IReflectionObject*);
            void* unk2;
            void* unk3;
            void* unk4;
            void* unk5;
            void* unk6;
            void* unk7;
            void* unk8;
            void* unk9;
        };
    };

    struct IndexedHandle {
        long m_id;
        gfl::Reflection* m_Reflection;
        gfl::IReflectionObject::instance* m_IObjectWithReflection;
        short m_weakReferenceCount;
        short m_instanceCount;
        char unk[0x4];
    };

    static_assert(sizeof(IndexedHandle) == 0x20);

    template <bool large = false>
    struct MemoryObject : ExternalType<MemoryObject<large>> {
        struct vtable {
            void* destructor;
            void* destructor2;
        };

        struct fields {
            gfl::SizedHeap* m_resource;
            void* m_ptr;
            std::conditional_t<large, int, short> m_size;
            std::conditional_t<large, int, short> m_align;
            char _pad[0x4];
        };

        static_assert(sizeof(fields) == 0x18);

        MemoryObject::instance* copy();
    };

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

    typedef void (*editRefType)(void* thiz);

    template <typename T = IReflectionObject>
    struct ObjectHandle {
        long m_id;
    };

    template <bool large = false>
    struct Object : ExternalType<Object<large>> {
        struct vtable : MemoryObject<large>::vtable {
            void* GetReflection;
            void* GetHandle;
            void* GetWeakHandle;
            void* unk4;
            void* unk5;
            void* unk6;
            void* unk7;
            void* unk8;
            editRefType OnZeroReference;
            editRefType IncrementReference;
            editRefType DecrementReference;
        };

        static_assert(sizeof(vtable) == 0x68);

        struct fields : MemoryObject<large>::fields {
            gfl::IObjectHandler::instance m_IObjectHandler;
            gfl::IReflectionObject::instance m_IObjectWithReflection;
            gfl::ObjectHandle<> m_handle;
            gfl::IndexedHandle* m_referenceCount;
        };

        static_assert(sizeof(fields) == 0x38);
    };

    template <bool large = false>
    struct ReferenceObject : ExternalType<ReferenceObject<large>> {
        struct vtable : gfl::MemoryObject<large>::vtable {
            editRefType OnZeroReference;
            editRefType IncrementReference;
            editRefType DecrementReference;
        };

        struct fields : gfl::MemoryObject<large>::fields {
            IReferenceObject::instance m_RefCountObject;
            int m_referenceCount;
            // char _pad[0x4];
        };
    };

    template <typename T>
    struct IntrusivePtr {
        typename T::instance* m_ptr;

        ~IntrusivePtr() {
            if (m_ptr != nullptr) {
                m_ptr->vtable->DecrementReference(m_ptr);
            }
        }
    };

    template <typename T>
    struct Singleton {
        void** vtable;
        gfl::SizedHeap* m_resource;

        static T s_instance;
    };
}
