#pragma once

#include "externals/util.h"

namespace sead {
    template <int N>
    struct WFixedSafeString {
        char16_t* mStringTop = &mBuffer;
        int mBufferSize = N;
        EXTERNAL_PAD(4);
        char16_t mBuffer[N];
    };
    static_assert(sizeof(char16_t) == 2);
}
