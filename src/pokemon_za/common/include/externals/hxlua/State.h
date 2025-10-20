#pragma once

#include "externals/gfl/string.h"

namespace hxlua {
    struct State {
        u8 unk0[0x68];
        lua_State* L;

        static int OnPanic(lua_State* L);

        void Load(char* buffer, uint64_t bufferSize, gfl::StringHolder* fileName);
    };

    static_assert(offsetof(State, L) == 0x68);
}
