#include <lua_ext/json.h>

#include "logger/logger.h"


static int lua_json_index(lua_State* L) {
    auto j = check_json(L, 1);

    if (j->is_object()) {
        if (lua_type(L, 2) == LUA_TSTRING) {
            const char* key = lua_tostring(L, 2);
            if (j->contains(key)) {
                push_json_value(L, (*j)[key]);
                return 1;
            }
        }
    } else if (j->is_array()) {
        if (lua_isnumber(L, 2)) {
            lua_Integer idx = lua_tointeger(L, 2);
            if (idx >= 1 && static_cast<size_t>(idx) <= j->size()) {
                push_json_value(L, (*j)[static_cast<size_t>(idx - 1)]);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

static int lua_json_newindex(lua_State* L) {
    auto j = check_json(L, 1);
    int val_type = lua_type(L, 3);
    nlohmann::json new_val;

    switch (val_type) {
        case LUA_TNIL:
            new_val = nullptr;
            break;
        case LUA_TBOOLEAN:
            new_val = static_cast<bool>(lua_toboolean(L, 3));
            break;
        case LUA_TNUMBER:
            if (lua_isinteger(L, 3)) {
                new_val = lua_tointeger(L, 3);
            } else {
                new_val = lua_tonumber(L, 3);
            }
            break;
        case LUA_TSTRING:
            new_val = lua_tostring(L, 3);
            break;
        case LUA_TUSERDATA: {
            void* ud = luaL_testudata(L, 3, LUA_JSON_METATABLE);
            if (ud != nullptr) {
                nlohmann::json* other = *static_cast<nlohmann::json**>(ud);
                new_val = *other;
            } else {
                return luaL_error(L, "Unsupported userdata type assigned to JSON object");
            }
            break;
        }
        case LUA_TTABLE: {
            new_val = lua_table_to_json(L, 3);
            break;
        }
        default:
            return luaL_error(L, "Unsupported value type assigned to JSON: %s", lua_typename(L, val_type));
    }

    if (j->is_object() && lua_type(L, 2) == LUA_TSTRING) {
        const char* key = lua_tostring(L, 2);
        if (val_type == LUA_TNIL) {
            j->erase(key);
        } else {
            (*j)[key] = new_val;
        }
    } else if (j->is_array() && lua_isnumber(L, 2)) {
        lua_Integer idx = lua_tointeger(L, 2);
        if (idx >= 1 && static_cast<size_t>(idx) <= j->size()) {
            if (val_type == LUA_TNIL) {
                j->erase(static_cast<size_t>(idx - 1));
            } else {
                (*j)[static_cast<size_t>(idx - 1)] = new_val;
            }
        } else if (idx == static_cast<lua_Integer>(j->size() + 1) && val_type != LUA_TNIL) {
            j->push_back(new_val);
        } else {
            return luaL_error(L, "Array index out of bounds: %lld", static_cast<long long>(idx));
        }
    } else {
        return luaL_error(L, "Attempted to mutate non-object/non-array JSON element or passed an invalid key key type");
    }

    return 0;
}

static int lua_json_tostring(lua_State* L) {
    auto j = check_json(L, 1);
    lua_pop(L, 1);
    std::string dump = j->dump();
    lua_pushfstring(L, "<JSON: %s>", dump.c_str());
    return 1;
}

static int lua_json_gc(lua_State* L) {
    auto ud = static_cast<nlohmann::json**>(luaL_checkudata(L, 1, LUA_JSON_OWNED_METATABLE));
    if (ud != nullptr && *ud != nullptr) {
        delete *ud;
        *ud = nullptr;
    }
    return 0;
}

static int lua_json_create(lua_State* L) {
    // Assume JSON already
    if (lua_type(L, 1) == LUA_TUSERDATA) return 1;

    luaL_checktype(L, 1, LUA_TTABLE);
    auto parsed_json = lua_table_to_json(L, 1);
    auto heap_json = new nlohmann::json(std::move(parsed_json));

    auto ud = static_cast<nlohmann::json**>(lua_newuserdata(L, sizeof(nlohmann::json*)));
    *ud = heap_json;

    luaL_getmetatable(L, LUA_JSON_OWNED_METATABLE);
    lua_setmetatable(L, -2);

    return 1;
}

static luaL_Reg meta[] = {
    {"__index", lua_json_index},
    {"__newindex", lua_json_newindex},
    {"__tostring", lua_json_tostring},
    {nullptr}
};

static luaL_Reg owned_meta[] = {
    {"__index", lua_json_index},
    {"__newindex", lua_json_newindex},
    {"__tostring", lua_json_tostring},
    {"__gc", lua_json_gc},
    {nullptr}
};

void lua_initjson(lua_State* L) {
    if (luaL_newmetatable(L, LUA_JSON_METATABLE)) {
        luaL_setfuncs(L, meta, 0);
    }
    lua_pop(L, 1);

    if (luaL_newmetatable(L, LUA_JSON_OWNED_METATABLE)) {
        luaL_setfuncs(L, owned_meta, 0);
    }
    lua_pop(L, 1);

    lua_pushcclosure(L, lua_json_create, 0);
    lua_setglobal(L, "json");
}

void lua_pushjson(lua_State* L, nlohmann::json& json) {
    auto ud = static_cast<nlohmann::json**>(lua_newuserdata(L, sizeof(nlohmann::json*)));
    *ud = &json;

    luaL_getmetatable(L, LUA_JSON_METATABLE);
    lua_setmetatable(L, -2);
}

void lua_pushjson_owned(lua_State* L, nlohmann::json& json) {
    auto ud = static_cast<nlohmann::json**>(lua_newuserdata(L, sizeof(nlohmann::json*)));
    *ud = &json;

    luaL_getmetatable(L, LUA_JSON_OWNED_METATABLE);
    lua_setmetatable(L, -2);
}
