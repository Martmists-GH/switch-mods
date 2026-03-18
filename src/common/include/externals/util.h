#pragma once

#include <hk/ro/RoUtil.h>
#include <concepts>

template <typename Ret, typename... Args>
static Ret external(long addr, Args... args) {
    return reinterpret_cast<Ret(*)(Args...)>(hk::ro::getMainModule()->range().start() + addr)(args...);
}

template<typename T>
concept ExternalSpec = requires {
    typename T::fields;
};

struct NoVTable {};

template<typename T>
concept HasVTable = requires {
    typename T::vtable;
};

template<typename T>
struct vtable_of {
    using type = NoVTable;
};

template<HasVTable T>
struct vtable_of<T> {
    using type = T::vtable;
};

template<typename T>
using vtable_of_t = vtable_of<T>::type;

template<typename A, typename B>
concept CompatibleVTable =
    (std::same_as<vtable_of_t<A>, NoVTable> && std::same_as<vtable_of_t<B>, NoVTable>) ||
        std::derived_from<vtable_of_t<A>, vtable_of_t<B>> ||
        std::derived_from<vtable_of_t<B>, vtable_of_t<A>>;

template<typename A, typename B>
concept CompatibleFields =
    std::derived_from<typename A::fields, typename B::fields> ||
    std::derived_from<typename B::fields, typename A::fields>;

template<typename A, typename B>
concept CompatibleExternalLayout =
    CompatibleVTable<A, B> &&
    CompatibleFields<A, B>;

template<typename T>
struct ExternalType {
    char _zeroSized[0];

    struct instance : T {
        static_assert(ExternalSpec<T>, "ExternalType<T>::instance requires T to define 'fields'");

        vtable_of_t<T> *vtable;
        T::fields fields;
    };

    template<typename Y> requires CompatibleExternalLayout<T, Y>
    constexpr Y::instance* castTo() {
        static_assert(ExternalSpec<T>);
        static_assert(ExternalSpec<Y>);
        return reinterpret_cast<Y::instance*>(this);
    }

    constexpr instance* impl() {
        static_assert(ExternalSpec<T>);
        return reinterpret_cast<instance*>(this);
    }
};

template<typename T>
struct ExternalInterface {
    char _zeroSized[0];

    struct instance : T {
        vtable_of_t<T> *vtable;
    };

    template<typename Y> requires CompatibleVTable<T, Y>
    constexpr Y::instance* castTo() {
        return reinterpret_cast<Y::instance*>(this);
    }

    constexpr instance* impl() {
        return reinterpret_cast<instance*>(this);
    }
};

#define CAT2(a,b) a##b
#define CAT(a,b) CAT2(a,b)
#define UNIQUE_ID CAT(_pad_,__COUNTER__)
#define EXTERNAL_PAD(size) char UNIQUE_ID[size]
#define DEBUG(str) hk::svc::OutputDebugString(str, strlen(str))
