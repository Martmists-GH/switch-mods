#pragma once

#include <unordered_map>
#include <unordered_set>

#include "pugixml.hpp"
#include "lua.hpp"
#include "logger/logger.h"
#undef cast
#define INJA_THROW(exception) { \
    Logger::log("%s", exception.what()); \
    abort(); \
}
#include "jsonutil.h"
#include "inja/inja.hpp"
#include "nlohmann/json.hpp"
#include "script.h"
#include "lua_ext/json.h"

namespace newui {
    class Renderer;
    struct CachedRenderNode;

    using RendererCallback = std::function<void(Renderer&, CachedRenderNode&, nlohmann::json&)>;

    template <typename T>
    struct __FunctionCallbackTraits {
        using type = std::function<void(nlohmann::json&, T, T)>;
    };

    template <>
    struct __FunctionCallbackTraits<void> {
        using type = std::function<void(nlohmann::json&)>;
    };

    template <typename T>
    using FunctionCallback = __FunctionCallbackTraits<T>::type;

    struct CachedRenderNode {
        pugi::xml_node node;
        RendererCallback cachedCallback;
        std::vector<CachedRenderNode> children;
        nlohmann::json cache;

        template <typename T>
        T get(std::string_view key, T defaultValue) {
            static_assert(!std::is_same_v<T, T>, "Unsupported type requested for CachedRenderNode::get()");
            return nullptr;
        }
    };

    template <>
    inline int CachedRenderNode::get<int>(std::string_view key, int defaultValue) {
        // return jsonutil::getInt(node, key, defaultValue);

        auto& el = cache[key];
        if (el.is_null()) {
            auto value = jsonutil::getInt(node, key, defaultValue);
            cache[key] = value;
            return value;
        }
        return el.get<int>();
    }

    template <>
    inline float CachedRenderNode::get<float>(std::string_view key, float defaultValue) {
        // return jsonutil::getFloat(node, key, defaultValue);

        auto& el = cache[key];
        if (el.is_null()) {
            auto value = jsonutil::getFloat(node, key, defaultValue);
            cache[key] = value;
            return value;
        }
        return el.get<float>();
    }

    template <>
    inline bool CachedRenderNode::get<bool>(std::string_view key, bool defaultValue) {
        // return jsonutil::getBool(node, key, defaultValue);

        auto& el = cache[key];
        if (el.is_null()) {
            auto value = jsonutil::getBool(node, key, defaultValue);
            cache[key] = value;
            return value;
        }
        return el.get<bool>();
    }

    template <>
    inline std::string_view CachedRenderNode::get<std::string_view>(std::string_view key, std::string_view defaultValue) {
        // return jsonutil::getString(node, key, defaultValue);

        auto& el = cache[key];
        if (el.is_null()) {
            auto value = jsonutil::getString(node, key, defaultValue);
            cache[key] = value;
            return value;
        }
        return el.get_ref<const std::string&>();
    }

    template <>
    inline ImVec2 CachedRenderNode::get<ImVec2>(std::string_view key, ImVec2 defaultValue) {
        auto& el = cache[key];
        if (el.is_null()) {
            auto value = jsonutil::getImVec2(node, key, defaultValue);
            cache[key] = {value.x, value.y};
            return value;
        }
        return jsonutil::getImVec2(cache, key, defaultValue);
    }

    template <>
    inline ImColor CachedRenderNode::get<ImColor>(std::string_view key, ImColor defaultValue) {
        auto& el = cache[key];
        if (el.is_null()) {
            auto value = jsonutil::getImColor(node, key, defaultValue);
            cache[key] = {value.Value.x, value.Value.y, value.Value.z, value.Value.w};
            return value;
        }
        return jsonutil::getImColor(cache, key, defaultValue);
    }






    class Renderer {
        std::string uiTemplate = "<Text content=\"<% ERROR: Renderer not initialized. %>\" />";
        nlohmann::json context = {};
        inja::Environment env = {};
        pugi::xml_document doc = {};
        CachedRenderNode cachedRoot;
        std::unordered_map<std::string, RendererCallback> renderers = {};

        std::unordered_map<std::string, FunctionCallback<void>> callbacksVoid = {};
        std::unordered_map<std::string, FunctionCallback<bool>> callbacksBoolean = {};
        std::unordered_map<std::string, FunctionCallback<int>> callbacksInt = {};
        std::unordered_map<std::string, FunctionCallback<float>> callbacksFloat = {};
        std::unordered_map<std::string, FunctionCallback<std::string&>> callbacksString = {};
        Renderer* parent = nullptr;

        bool m_needsUpdate = false;

        void initContext();
        void initTypes();
        void initCallbacks();

        template <typename T>
        auto& getCallbackMap() {
            if constexpr (std::is_void_v<T>) return callbacksVoid;
            else if constexpr (std::is_same_v<T, bool>) return callbacksBoolean;
            else if constexpr (std::is_same_v<T, int>) return callbacksInt;
            else if constexpr (std::is_same_v<T, float>) return callbacksFloat;
            else if constexpr (std::is_same_v<T, std::string>) return callbacksString;
            else if constexpr (std::is_same_v<T, std::string&>) return callbacksString;
        }

    public:
        static constexpr const char* PLACEHOLDER = "$PLACEHOLDER$";

        Renderer(Renderer* parent = nullptr);

        Renderer& root();

        template <typename T>
        void registerCallback(const std::string& name, FunctionCallback<T> callback) {
            root().getCallbackMap<T>()[name] = std::move(callback);
        }
        void registerRenderer(const std::string& type, RendererCallback callback);

        void registerEnv(const std::function<void(inja::Environment&)>& callback);
        void registerLua(const std::function<void(lua_State*)>& callback);

        template <typename T>
        bool hasCallback(const std::string& name) {
            return root().getCallbackMap<T>().contains(name);
        }

        template <typename T>
        bool invokeLua(const std::string& name, nlohmann::json& ctx, const std::string& type, T* before, T* after) {
            auto L = obtainLuaState();

            lua_pushlightuserdata(L, this);
            lua_setglobal(L, "renderer");

            lua_pushjson(L, ctx);
            lua_setglobal(L, "context");

            lua_newtable(L);

            lua_pushstring(L, type.c_str());
            lua_setfield(L, -2, "type");

            auto pushValue = [L](const T* val) {
                if (!val) {
                    lua_pushnil(L);
                    return;
                }
                if constexpr (std::is_same_v<T, bool>) {
                    lua_pushboolean(L, *val);
                } else if constexpr (std::is_integral_v<T>) {
                    lua_pushinteger(L, *val);
                } else if constexpr (std::is_floating_point_v<T>) {
                    lua_pushnumber(L, *val);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    lua_pushstring(L, val->c_str());
                } else {
                    lua_pushnil(L);
                }
            };

            pushValue(before);
            lua_setfield(L, -2, "before");

            pushValue(after);
            lua_setfield(L, -2, "after");

            lua_setglobal(L, "event");

            auto expr = "return " + name;
            if (luaL_loadstring(L, expr.c_str()) == LUA_OK) {
                if (lua_pcall(L, 0, 1, 0) == LUA_OK) {
                    if (lua_isfunction(L, -1)) {
                        lua_pushvalue(L, 1);

                        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
                            reportLuaError(L, lua_tostring(L, -1));
                            lua_pop(L, 2);
                            return false;
                        }

                        lua_pop(L, 1);
                    } else {
                        lua_pop(L, 1);
                    }

                    return true;
                }

                lua_pop(L, 1);
            } else {
                lua_pop(L, 1);
            }

            if (luaL_loadstring(L, name.c_str()) == LUA_OK) {
                if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
                    reportLuaError(L, lua_tostring(L, -1));
                    lua_pop(L, 2);
                    return false;
                }
            } else {
                reportLuaError(L, lua_tostring(L, -1));
                lua_pop(L, 2);
                return false;
            }

            lua_pop(L, 1);
            return true;
        }

        template <typename T>
        void invokeCallback(std::string_view name, const std::string& type, nlohmann::json& ctx, T before, T after) {
            auto nameString = std::string(name);
            auto& map = getCallbackMap<T>();
            if (map.contains(nameString)) {
                map[nameString](ctx, before, after);
            } else {
                invokeLua(nameString, ctx, type, &before, &after);
            }
        }

        void invokeCallback(std::string_view name, const std::string& type, nlohmann::json& ctx, std::string_view before, std::string_view after) {
            auto nameString = std::string(name);
            auto& map = getCallbackMap<std::string&>();

            auto b = std::string(before);
            auto a = std::string(after);

            if (map.contains(nameString)) {
                map[nameString](ctx, b, a);
            } else {
                invokeLua(nameString, ctx, type, &b, &a);
            }
        }

        void invokeCallback(std::string_view name, const std::string& type, nlohmann::json& ctx) {
            auto nameString = std::string(name);
            if (callbacksVoid.contains(nameString)) {
                callbacksVoid[nameString](ctx);
            } else {
                invokeLua<void>(nameString, ctx, type, nullptr, nullptr);
            }
        }

        void init(const char* path);
        bool needsUpdate() const;
        void markNeedsUpdate();

        void render();
        void render(nlohmann::json& ctx);
        CachedRenderNode buildCache(const pugi::xml_node& node);
        void render(CachedRenderNode& node, nlohmann::json& ctx);

        // AVOID USE, ONLY DESIGNED FOR <Include /> TAGS
        std::unordered_map<std::string, std::unique_ptr<Renderer>> includeCache = {};
        // Cached parsed JSON for <Include context="..."/> attributes, keyed by (source + context)
        std::unordered_map<std::string, nlohmann::json> includeContextCache = {};
    };
}
