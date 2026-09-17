#pragma once

#include <lua.hpp>

#include "nlohmann/json.hpp"


static const char* LUA_JSON_METATABLE = "nlohmann_json_userdata";
static const char* LUA_JSON_OWNED_METATABLE = "nlohmann_json_owned_userdata";

void lua_initjson(lua_State* L);
void lua_pushjson(lua_State* L, nlohmann::json& obj);


static nlohmann::json* check_json(lua_State* L, int arg) {
    auto ud = static_cast<nlohmann::json**>(luaL_checkudata(L, arg, LUA_JSON_METATABLE));
    if (ud == nullptr) {
        ud = static_cast<nlohmann::json**>(luaL_checkudata(L, arg, LUA_JSON_OWNED_METATABLE));
    }

    luaL_argcheck(L, ud != nullptr && *ud != nullptr, arg, "Valid JSON reference expected");
    return *ud;
}

static void push_json_value(lua_State* L, nlohmann::json& val) {
    if (val.is_null()) {
        lua_pushnil(L);
    } else if (val.is_boolean()) {
        lua_pushboolean(L, val.get<bool>() ? 1 : 0);
    } else if (val.is_number_integer()) {
        lua_pushinteger(L, val.get<lua_Integer>());
    } else if (val.is_number_float()) {
        lua_pushnumber(L, val.get<lua_Number>());
    } else if (val.is_string()) {
        const std::string& str = val.get_ref<const std::string&>();
        lua_pushlstring(L, str.data(), str.size());
    } else if (val.is_object() || val.is_array()) {
        lua_pushjson(L, val);
    } else {
        lua_pushnil(L);
    }
}

static nlohmann::json lua_table_to_json(lua_State* L, int index) {
    auto type = lua_type(L, index);

    switch (type) {
    case LUA_TNIL:
        return nullptr;
    case LUA_TBOOLEAN:
        return static_cast<bool>(lua_toboolean(L, index));
    case LUA_TNUMBER:
        if (lua_isinteger(L, index)) {
            return lua_tointeger(L, index);
        }
        return lua_tonumber(L, index);
    case LUA_TSTRING:
        return lua_tostring(L, index);
    case LUA_TUSERDATA: {
        void* ud = luaL_testudata(L, index, LUA_JSON_METATABLE);
        if (!ud) ud = luaL_testudata(L, index, LUA_JSON_OWNED_METATABLE);
        if (ud != nullptr && *static_cast<nlohmann::json**>(ud) != nullptr) {
            return **static_cast<nlohmann::json**>(ud);
        }
        return nullptr;
    }
    case LUA_TTABLE: {
        nlohmann::json j;
        size_t len = lua_rawlen(L, index);
        if (len > 0) {
            j = nlohmann::json::array();
            for (size_t i = 1; i <= len; ++i) {
                lua_rawgeti(L, index, static_cast<lua_Integer>(i));
                j.push_back(lua_table_to_json(L, -1));
                lua_pop(L, 1);
            }
        } else {
            j = nlohmann::json::object();
            lua_pushnil(L);
            while (lua_next(L, index) != 0) {
                if (lua_type(L, -2) == LUA_TSTRING) {
                    const char* key = lua_tostring(L, -2);
                    j[key] = lua_table_to_json(L, -1);
                } else if (lua_isnumber(L, -2)) {
                    std::string key = std::to_string(lua_tointeger(L, -2));
                    j[key] = lua_table_to_json(L, -1);
                }
                lua_pop(L, 1);
            }
        }
        return j;
    }
    default:
        return nullptr;
    }
}
