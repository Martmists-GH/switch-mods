#pragma once

namespace hxlua {
    enum class ObjectHolderType : long {
        RawPtr,
        SharedPtr,
        Unk2,
        IndexedPtr
    };

    template <typename T>
    struct ObjectHolder {
        ObjectHolderType m_type;
        union {
            T* m_ptr;
            T* m_sharedPtr;
            void* m_unk2;
            gfl::IndexedHandle* m_indexPtr;
        };

        T* Get() {
            switch (m_type) {
                case ObjectHolderType::RawPtr:
                    return m_ptr;
                case ObjectHolderType::SharedPtr:
                    return m_sharedPtr;
                case ObjectHolderType::Unk2:  // This one is just a placeholder, doesn't seem to ever be used?
                    return (T*)m_unk2;
                case ObjectHolderType::IndexedPtr: {
                    auto ref = m_indexPtr->m_IObjectWithReflection;
                    return reinterpret_cast<T*>(reinterpret_cast<ulong>(ref) - offsetof(T, fields.m_IObjectWithReflection));
                }
            }
        }
    };

    struct helper {
        template <typename T>
        static inline ObjectHolder<T>* ToTypedUserData(lua_State* L, int arg) {
            auto holderPtr = lua_touserdata(L, arg);
            if (holderPtr == nullptr) {
                return nullptr;
            }
            return *static_cast<hxlua::ObjectHolder<T>**>(holderPtr);
        }
    };
}
