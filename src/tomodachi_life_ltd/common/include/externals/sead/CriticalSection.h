#pragma once
#include "IDisposer.h"

namespace sead {
    struct CriticalSection : ExternalType<CriticalSection> {
        struct vtable : sead::IDisposer::vtable {};
        struct fields : sead::IDisposer::fields {
            char m_mutex[0x20];
        };
    };
}
