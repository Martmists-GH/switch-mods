#pragma once
#include <lua.hpp>

lua_State* get_lua_state();

#define luaL_dostringfile(L, s, f) \
    (luaL_loadstring(L, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

#define executeLua(L, contents, cause) \
    if (luaL_dostringfile(L, contents, cause) != LUA_OK) {\
        Logger::log("An error occurred in %s: %s\n", cause, lua_tostring(L, -1));\
    }
