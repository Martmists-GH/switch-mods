#include "imgui.h"
#include "newui/newui.h"

static void renderLuaValue(lua_State* L, int idx) {
    int type = lua_type(L, idx);
    switch (type) {
    case LUA_TNIL:
        ImGui::TextDisabled("nil");
        break;

    case LUA_TBOOLEAN:
        ImGui::TextUnformatted(lua_toboolean(L, idx) ? "true" : "false");
        break;

    case LUA_TNUMBER:
        if (lua_isinteger(L, idx)) {
            ImGui::Text("%lld", static_cast<long long>(lua_tointeger(L, idx)));
        } else {
            ImGui::Text("%f", lua_tonumber(L, idx));
        }
        break;

    case LUA_TSTRING:
        ImGui::TextUnformatted(lua_tostring(L, idx));
        break;

    case LUA_TTABLE:
        ImGui::Text("table: %p", lua_topointer(L, idx));
        break;

    case LUA_TFUNCTION:
        ImGui::Text("function: %p", lua_topointer(L, idx));
        break;

    case LUA_TUSERDATA:
    case LUA_TLIGHTUSERDATA:
        ImGui::Text("userdata: %p", lua_topointer(L, idx));
        break;

    case LUA_TTHREAD:
        ImGui::Text("thread: %p", lua_topointer(L, idx));
        break;

    default:
        ImGui::Text("%s: %p", lua_typename(L, type), lua_topointer(L, idx));
        break;
    }
}

namespace newui {
    NEWUI_ELEMENT(Include) {
        auto source = std::string(jsonutil::getString(node, "source", ""));
        auto extraContext = jsonutil::getString(node, "context", "{}");

        auto cacheKey = source + "\x00" + std::string(extraContext);
        auto& cache = renderer.root().includeContextCache;
        if (!cache.contains(cacheKey)) {
            cache[cacheKey] = nlohmann::json::parse(extraContext);
        }
        auto& ctx = cache[cacheKey];

        for (auto elem : ctx.items()) {
            context[elem.key()] = elem.value();
        }

        if (source.empty()) {
            ImGui::TextUnformatted("<% ERROR: Missing attribute 'source' %>");
        } else {
            if (!renderer.includeCache.contains(source)) {
                Logger::log("Creating new renderer for include %s", source.c_str());
                auto child = std::make_unique<Renderer>(&renderer);
                child->init(source.c_str());
                renderer.includeCache[source] = std::move(child);
            }

            renderer.includeCache[source]->render(context);
        }
    }

    NEWUI_ELEMENT(Text) {
        auto content = jsonutil::getString(node, "content", "");
        auto script = jsonutil::getString(node, "contentscript", "");
        if (content.empty() && script.empty()) {
            ImGui::TextUnformatted("<% ERROR: Missing attribute 'content' %>");
            return;
        }

        if (content.empty()) {
            auto L = obtainLuaState();

            lua_pushlightuserdata(L, &renderer.root());
            lua_setglobal(L, "renderer");

            lua_pushjson(L, context);
            lua_setglobal(L, "context");

            auto expr = "return " + std::string(script);
            if (luaL_loadstring(L, expr.c_str()) == LUA_OK) {
                if (lua_pcall(L, 0, 1, 0) == LUA_OK) {
                    if (lua_isfunction(L, -1)) {
                        lua_pushvalue(L, 1);

                        if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
                            ImGui::TextUnformatted(lua_tostring(L, -1));
                            lua_pop(L, 2);
                        } else {
                            // Logger::log("Result: %d - %s", lua_type(L, -1), lua_tostring(L, -1));
                            renderLuaValue(L, -1);
                            lua_pop(L, 2);
                        }
                    } else {
                        // Logger::log("Result: %d - %s", lua_type(L, -1), lua_tostring(L, -1));
                        renderLuaValue(L, -1);
                        lua_pop(L, 1);
                    }
                } else {
                    // Logger::log("pcall error: %s", lua_tostring(L, -1));
                    ImGui::TextUnformatted(lua_tostring(L, -1));
                    lua_pop(L, 1);
                }
            } else {
                // Logger::log("Loadstring error: %s", lua_tostring(L, -1));
                ImGui::TextUnformatted(lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        } else {
            ImGui::TextUnformatted(content.data());
        }
    }

    NEWUI_ELEMENT(TextDisabled) {
        auto content = jsonutil::getString(node, "content", "<% ERROR: Missing attribute 'content' %>");
        ImGui::TextDisabled("%s", content.data());
    }

    NEWUI_ELEMENT(Separator) {
        ImGui::Separator();
    }

    NEWUI_ELEMENT(SeparatorText) {
        auto content = jsonutil::getString(node, "content", "<% ERROR: Missing attribute 'content' %>");
        ImGui::SeparatorText(content.data());
    }

    NEWUI_ELEMENT(Spacing) {
        ImGui::Spacing();
    }

    NEWUI_ELEMENT(Callback) {
        auto onFrame = jsonutil::getString(node, "onframe", "");

        if (onFrame.empty()) {
            ImGui::TextUnformatted("<% ERROR: Missing attribute 'onframe' %>");
        } else {
            renderer.invokeCallback(onFrame, "onframe", context);
        }
    }
}
