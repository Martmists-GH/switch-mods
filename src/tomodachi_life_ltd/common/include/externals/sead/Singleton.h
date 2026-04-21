#pragma once

namespace sead {
    template <typename T>
    struct Singleton : ExternalType<T> {
        static T* s_instance;
    };
}
