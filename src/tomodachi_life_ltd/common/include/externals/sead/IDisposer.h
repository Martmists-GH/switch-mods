#pragma once

#include "externals/util.h"

namespace sead {
    struct Heap;

    struct IDisposer : ExternalType<IDisposer> {
        struct vtable {
            void* destructor;
            void* destructor2;
        };

        struct fields {
            Heap* m_disposerHeap;
            char m_listNode[0x10];
        };
    };
}
