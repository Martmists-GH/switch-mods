#pragma once
#include <checks.hpp>
#include <nn/os.h>
#include "externals/util.h"

// TODO: A lot of these are wild guesses and likely wrong, and need a lot of cleanup
namespace gfl {
    struct SizedHeap;

    struct IRefCountObject : ExternalInterface<IRefCountObject> {
        struct vtable {
            void* unk1;  // Destructor?
            void* unk2;  // Destructor?
            void* IncrementReference;
            void* DecrementReference;
        };
    };

    struct IObjectHandler : ExternalInterface<IObjectHandler> {
        struct vtable : gfl::IRefCountObject::vtable {
            void* GetHandle;
            void* GetWeakHandle;
            void* unk7;  // GetReflection?
            void* unk8;
        };
    };

    struct Reflection {
        gfl::Reflection* m_parent;
    };

    struct IObjectWithReflection : ExternalType<IObjectWithReflection> {
        struct vtable {
            gfl::Reflection*(*GetReflection)(gfl::IObjectWithReflection*);
            void* unk2;
            void* unk3;
            void* unk4;
            void* unk5;
            void* unk6;
            void* unk7;
            void* unk8;
            void* unk9;
        };

        struct fields { };
    };

    struct IndexedHandle {
        long m_id;
        gfl::Reflection* m_Reflection;
        gfl::IObjectWithReflection::instance* m_IObjectWithReflection;
        short m_weakReferenceCount;
        short m_instanceCount;
        char unk[0x4];
    };

    static_assert(sizeof(IndexedHandle) == 0x20);

    struct AllocatedObject : ExternalType<AllocatedObject> {
        struct vtable {
            void* destructor;
            void* destructor2;
        };

        struct fields {
            gfl::SizedHeap* m_resource;
            void* m_ptr;
            short m_size;
            short m_align;
            char _pad[0x4];
        };

        static_assert(sizeof(fields) == 0x18);

        AllocatedObject::instance* copy();
    };

    struct SizedHeap : ExternalType<SizedHeap> {
        struct vtable {
            void* unk1;
            void* unk2;
            void*(*allocate)(gfl::SizedHeap*, int size, int align);
            void* deallocate;
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
            char unk2[0x38];
            uint64_t allocSize;
            uint64_t allocMax;
        };
    };

    struct HeapManager : ExternalType<HeapManager> {
        struct childHeap {
            SizedHeap* m_heap;
            SizedHeap* m_parentHeap;
            void* unk1;
            long unk2;
        };

        struct vtable {
            // TODO
        };
        struct fields {
            childHeap heaps[4];
        };


        static HeapManager* GetInstance();
    };

    template <typename T>
    struct TypedHandle {
        gfl::IndexedHandle* m_handle;
        long m_id;


        T* Get() {
            auto ref = m_handle->m_IObjectWithReflection;
            // Requires T to have `fields` with `m_IReflectionObject`
            return static_cast<T*>(reinterpret_cast<ulong>(ref) - offsetof(T, fields.m_IReflectionObject));
        }
    };

    static_assert(sizeof(TypedHandle<void>) == 0x10);

    struct Object : ExternalType<Object> {
        struct vtable : AllocatedObject::vtable {
            void* GetReflection;
            void* GetHandle;
            void* GetWeakHandle;
            void* unk4;
            void* unk5;
            void* unk6;
            void* unk7;
            void* unk8;
            void* OnZeroReference;
            void* IncrementReference;
            void* DecrementReference;
        };

        static_assert(sizeof(vtable) == 0x68);

        struct fields : AllocatedObject::fields {
            gfl::IObjectHandler::instance* m_IObjectHandler;
            gfl::IObjectWithReflection::instance* m_IObjectWithReflection;
            gfl::TypedHandle<gfl::IObjectWithReflection> m_handle;
        };

        static_assert(offsetof(fields, m_IObjectWithReflection) == 0x20);
        static_assert(sizeof(fields) == 0x38);
    };

    static_assert(sizeof(Object::instance) == 0x40);

    typedef void (*editRefType)(void* thiz);

    struct ReferenceObject : ExternalType<ReferenceObject> {
        struct vtable : gfl::AllocatedObject::vtable {
            editRefType OnZeroReference;
            editRefType IncrementReference;
            editRefType DecrementReference;
        };

        struct fields : gfl::AllocatedObject::fields {
            IRefCountObject::instance m_RefCountObject;
            int m_referenceCount;
            // char _pad[0x4];
        };
    };

    template <typename T = ReferenceObject>
    struct RefPointer {
        typename T::instance* m_ptr;

        ~RefPointer() {
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
