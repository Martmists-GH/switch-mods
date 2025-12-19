#pragma once

#include <hk/ro/RoUtil.h>
#include <concepts>

template <typename Ret, typename... Args>
static Ret external(long addr, Args... args) {
    return reinterpret_cast<Ret(*)(Args...)>(hk::ro::getMainModule()->range().start() + addr)(args...);
}

template<typename T>
struct ExternalType {
    char _zeroSized[0];

    struct vtable { char _zeroSized[0]; };
    struct fields { char _zeroSized[0]; };

    struct instance : T {
        T::vtable *vtable;
        T::fields fields;
    };

    template<typename Y>
    inline constexpr typename Y::instance* castTo() {
        static_assert(std::derived_from<typename Y::vtable, typename T::vtable> || std::derived_from<typename T::vtable, typename Y::vtable>);
        static_assert(std::derived_from<typename Y::fields, typename T::fields> || std::derived_from<typename T::fields, typename Y::fields>);
        return reinterpret_cast<typename Y::instance*>(this);
    }

    inline constexpr struct instance* impl() {
        return reinterpret_cast<struct instance*>(this);
    }
};

template<typename T>
struct ExternalInterface {
    char _zeroSized[0];

    struct vtable { char _zeroSized[0]; };

    struct instance : T {
        T::vtable *vtable;
    };

    template<typename Y>
    inline constexpr typename Y::instance* castTo() {
        static_assert(std::derived_from<typename Y::vtable, typename T::vtable> || std::derived_from<typename T::vtable, typename Y::vtable>);
        static_assert(std::derived_from<typename Y::fields, typename T::fields> || std::derived_from<typename T::fields, typename Y::fields>);
        return reinterpret_cast<typename Y::instance*>(this);
    }

    inline constexpr struct instance* impl() {
        return reinterpret_cast<struct instance*>(this);
    }
};

#define CAT2(a,b) a##b
#define CAT(a,b) CAT2(a,b)
#define UNIQUE_ID CAT(_pad_,__COUNTER__)
#define EXTERNAL_PAD(size) char UNIQUE_ID[size]
