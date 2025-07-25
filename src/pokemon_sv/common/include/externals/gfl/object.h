#pragma once
#include <checks.hpp>
#include <nn/os.h>
#include "externals/util.h"

// TODO: A lot of these are wild guesses and likely wrong, and need a lot of cleanup
namespace gfl {
    struct SizedHeap;

    struct ThreadLocalHelper {
        struct _stack {
            gfl::SizedHeap* stack[0x10];
            long stackSize;

            void push(gfl::SizedHeap* res) {
                stack[stackSize++] = res;
            }

            gfl::SizedHeap* pop() {
                return stack[--stackSize];
            }
        };

        _stack m_heapStack;

        static ThreadLocalHelper* GetInstance() {
            auto offset = 0;
            if (is_version("3.0.1")) {
                offset = 0x04765a30;
            } else {
                HK_ABORT("ThreadLocalHelper::GetInstance Not implemented for version!", nullptr);
            }
            auto tlsPtr = reinterpret_cast<nn::os::TlsSlot*>(hk::ro::getMainModule()->range().start() + offset);
            return reinterpret_cast<ThreadLocalHelper*>(nn::os::GetTlsValue(*tlsPtr));
        }

        static void PushHeap(gfl::SizedHeap* heap) {
            auto _this = GetInstance();
            _this->m_heapStack.push(heap);
        }

        static void PopHeap(gfl::SizedHeap* resource) {
            auto _this = GetInstance();
            _this->m_heapStack.pop();
        }
    };

    struct IRefCountObject : ExternalType<IRefCountObject> {
        struct vtable {
            void* unk1;  // Destructor?
            void* unk2;  // Destructor?
            void* IncrementReference;
            void* DecrementReference;
        };

        struct fields { };
    };

    struct IObjectHandler : ExternalType<IObjectHandler> {
        struct vtable : gfl::IRefCountObject::vtable {
            void* GetHandle;
            void* GetWeakHandle;
            void* unk7;  // GetReflection?
            void* unk8;
        };

        struct fields : gfl::IRefCountObject::fields { };
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
            void* unk6;  // Destructor?
            void* unk7;
            void* unk8;
            void* unk9;
        };

        struct fields { };
    };

    struct IndexedHandle {  // Assumed from "C:/jenkins/workspace/sudachi/nuget/nuget_trinity_build_s2/trinity_titan/lib/gflib3/include\\core/indexed_handle.h"
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
            char* unk1;
            bool unk2;
            bool unk3;
        };

        template <typename T, typename ...Args>
        typename T::instance* Create(Args&& ...args) {
            gfl::ThreadLocalHelper::PushHeap(this);
            auto impl = this->impl();
            auto obj = (typename T::instance*)(impl->vtable->allocate(impl, sizeof(typename T::instance), alignof(typename T::instance)));
            obj->$ctor(std::forward<Args>(args)...);
            auto mem = ((gfl::AllocatedObject*)obj)->impl();
            mem->fields.m_resource = this;
            mem->fields.m_size = sizeof(typename T::instance);
            mem->fields.m_align = 8;
            mem->fields.m_ptr = obj;
            impl->vtable->unk29(impl);
            gfl::ThreadLocalHelper::PopHeap(this);
            return obj;
        }
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

    struct ReferenceObject : ExternalType<ReferenceObject> {  // "C:/jenkins/workspace/sudachi/nuget/nuget_battleLogic_build_s2/battleLogic_titan/prog/../lib/gflib3/include\\core/reference_object.h"
        struct vtable : gfl::Object::vtable {
            void* OnZeroReference;
            editRefType IncrementReference;
            editRefType DecrementReference;
        };

        struct fields : gfl::Object::fields {
            int m_referenceCount;
            char _pad[0x4];
        };

        static_assert(sizeof(fields) == 0x40);
    };
}
