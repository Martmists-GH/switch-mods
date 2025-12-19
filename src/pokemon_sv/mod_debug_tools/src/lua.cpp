#include "lua.hpp"
#undef cast

#include <externals/gfl/text.h>
#include <externals/hxlua/helper.h>
#include <logger/logger.h>

static int luaGetGflString(lua_State* L) {
    auto holder = hxlua::helper::ToTypedUserData<gfl::text::StrBuf::instance>(L, 1);
    auto text = holder->Get();
    auto str = text->asString();

    lua_pushstring(L, str.c_str());
    return 1;
}

void lua_mod_code(lua_State* L) {
    lua_pushglobaltable(L);

    lua_pushcfunction(L, luaGetGflString);
    lua_setfield(L, -2, "gfl_string");

    lua_pop(L, 1);
}
