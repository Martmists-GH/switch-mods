#include <lua.hpp>

#include "logger/logger.h"

extern "C" int luaB_print_impl(lua_State* L) {
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

    return 0;
}
