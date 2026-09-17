#include <hk/hook/Trampoline.h>
#include <logger/logger.h>
#include <nn/fs/fs_directories.h>
#include <nn/fs/fs_files.h>
#include <nn/fs/fs_types.h>
#include <lua.hpp>

#include "externals/hxlua/lua_baselib.h"
#include "externals/hxlua/State.h"

#include "lua_utils.h"
#include "game_constants.h"
#include "lua_ext/directory_loader.h"

static lua_State* gLuaState = nullptr;
lua_State* get_lua_state() {
    return gLuaState;
}

void lua_mod_code(lua_State *L);

HkTrampoline<int, lua_State*> LuaPrintHook = hk::hook::trampoline([](lua_State *L) {
    const int n = lua_gettop(L);
    std::string s = "Lua Print:";
    for (int i = 1; i <= n; i++) {
        size_t size;
        const char *message = lua_tolstring(L, i, &size);
        s.append(" ");
        s.append(message, size);
    }
    lua_settop(L, 0);
    s.append("\n");
    Logger::log(s);

    return LuaPrintHook.orig(L);
});

HkTrampoline<void, lua_State*> LuaPanicHook = hk::hook::trampoline([](lua_State *L) {
    size_t size;
    auto message = lua_tolstring(L, 1, &size);
    Logger::log("Lua Panic: %.*s", size, message);
    LuaPanicHook.orig(L);
});

HkTrampoline<void, lua_State*> LuaAbortHook = hk::hook::trampoline([](lua_State *L) {
    auto condition = lua_toboolean(L, 1);
    size_t size;
    auto message = lua_tolstring(L, 2, &size);
    if (!condition) {
        Logger::log("Lua Abort: %.*s", size, message);
    }
    LuaAbortHook.orig(L);
});

// Trimmed down to only what we use
static TValue *index2value(lua_State *L, int idx) {
    CallInfo *ci = L->ci;
    StkId o = ci->func + idx;
    api_check(L, idx <= L->ci->top - (ci->func + 1), "unacceptable index");
    if (o >= L->top) return &G(L)->nilvalue;
    else return s2v(o);
}

static int luaGetFnPtr(lua_State *L) {
    const TValue *o = index2value(L, 1);

    long x;
    if (ttislcf(o)) {
        x = (ulong) fvalue(o);
    } else if (ttisCclosure(o)) {
        x = (ulong) clCvalue(o)->f;
    } else {
        x = 0;
    }
    if (x != 0) {
        lua_pushinteger(L, x - hk::ro::getMainModule()->range().start());
    } else {
        lua_pushinteger(L, 0);
    }
    return 1;
}

void registerLuaUtils(lua_State *L) {
    luaL_requiref(L, "debug", luaopen_debug, 1);
    lua_pushglobaltable(L);

    lua_pushcfunction(L, reinterpret_cast<lua_CFunction>(luaGetFnPtr));
    lua_setfield(L, -2, "_FuncPtr");

    lua_pop(L, 1);
}

static int counter = 0;
HkTrampoline<void, hxlua::State*, char*, uint64_t, gfl::StringView*> AfterMainHook = hk::hook::trampoline([](hxlua::State* state, char* buffer, uint64_t bufferSize, gfl::StringView* fileName){
    AfterMainHook.orig(state, buffer, bufferSize, fileName);

    auto L = state->L;
    gLuaState = L;

    Logger::log("AfterMainHook executing file: %s", fileName->m_content.m_string);

    if (++counter == 2) {
        registerLuaUtils(L);
        lua_mod_code(L);

        executeScriptsInDirectory(L, ROM_MOUNT ":/script/lua/boot");
    }
});

void lua_hooks() {
    LuaPrintHook.installAtPtr(hxlua::luaB_print);
    LuaPanicHook.installAtPtr(hxlua::State::OnPanic);
    AfterMainHook.installAtPtr(pun<void*>(&hxlua::State::Load));
}
