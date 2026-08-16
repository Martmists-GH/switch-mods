#pragma once

struct lua_State;

namespace newui {
    lua_State* obtainLuaState();
    void initializeLuaState(lua_State*);
    void reportLuaError(lua_State*, const char*);
}
