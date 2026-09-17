#pragma once

#include "script.h"

#include "lua_ext/json.h"
#include "logger/logger.h"
#include "lua_ext/ffi.h"
#include "lua_ext/directory_loader.h"

static int lua_log(lua_State* L) {
    auto log = lua_tostring(L, 1);
    Logger::log("%s", log);
    return 0;
}

static int lua_panic(lua_State* L) {
    Logger::log("Lua Panic: %s", lua_tostring(L, -1));
    abort();
}

static lua_State* gState = nullptr;

namespace newui {
    lua_State* obtainLuaState() {
        if (gState == nullptr) {
            gState = luaL_newstate();

            luaL_requiref(gState, "_G", luaopen_base, 1);
            lua_settop(gState, -2);
            luaL_requiref(gState, "table", luaopen_table, 1);
            lua_settop(gState, -2);
            luaL_requiref(gState, "string", luaopen_string, 1);
            lua_settop(gState, -2);
            luaL_requiref(gState, "math", luaopen_math, 1);
            lua_settop(gState, -2);

            lua_atpanic(gState, lua_panic);
            initializeLuaState(gState);
        }

        return gState;
    }

    __attribute__((weak))
    void initializeLuaState(lua_State* L) {
        luaopen_ffi(L);

        lua_initjson(L);

        lua_pushcfunction(L, lua_log);
        lua_setglobal(L, "log");

        executeScriptsInDirectory(L, ROM_MOUNT ":/ui/script");
    }

    __attribute__((weak))
    void reportLuaError(lua_State*, const char* err) {
        Logger::log("%s", err);
    }
}
