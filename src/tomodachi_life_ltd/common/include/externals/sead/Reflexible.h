#pragma once

namespace sead {
    struct RTTI {
        void** vtable;
    };

    struct Reflexible : ExternalInterface<Reflexible> {
        struct vtable {
            RTTI*(*checkDerivedRuntimeTypeInfo)(void*, sead::RTTI*);
            RTTI*(*getRuntimeTypeInfo)();
        };
    };
}
