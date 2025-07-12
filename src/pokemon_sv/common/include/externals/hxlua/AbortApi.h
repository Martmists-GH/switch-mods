#pragma once

namespace hxlua {
    struct AbortApi {
        static void Abort(lua_State* L);
    };
}
