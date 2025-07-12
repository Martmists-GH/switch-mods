#pragma once

#include <lua.h>

// Custom namespace to avoid overlap with lib/lua
namespace hxlua {
    static int luaB_print(lua_State *L);
}
