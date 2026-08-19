#include "Renderer.h"

#include "game_constants.h"
#include "imgui.h"
#include "newui.h"
#include "hk/types.h"
#include "hk/ro/RoUtil.h"
#include "logger/logger.h"
#include "lua_ext/ffi.h"
#include "util/FileUtil.h"

static inline char* substringAfter(char* s, char c) {
    auto ptr = strchr(s, c);
    if (ptr != nullptr) {
        return ptr + 1;
    }
    return s;
}

namespace newui {
    NEWUI_ELEMENT(Root);
    NEWUI_ELEMENT(Window);
    NEWUI_ELEMENT(Columns);
    NEWUI_ELEMENT(Row);
    NEWUI_ELEMENT(Column);
    NEWUI_ELEMENT(Child);
    NEWUI_ELEMENT(CollapsingHeader);
    NEWUI_ELEMENT(Group);

    NEWUI_ELEMENT(Include);
    NEWUI_ELEMENT(Text);
    NEWUI_ELEMENT(TextDisabled);
    NEWUI_ELEMENT(Separator);
    NEWUI_ELEMENT(SeparatorText);
    NEWUI_ELEMENT(Spacing);
    NEWUI_ELEMENT(Callback);

    NEWUI_ELEMENT(Button);
    NEWUI_ELEMENT(Combo);
    NEWUI_ELEMENT(Selectable);
    NEWUI_ELEMENT(Checkbox);
    NEWUI_ELEMENT(InputInt);
    NEWUI_ELEMENT(SliderInt);
    NEWUI_ELEMENT(InputFloat);
    NEWUI_ELEMENT(SliderFloat);

    NEWUI_ELEMENT(MainMenuBar);
    NEWUI_ELEMENT(MenuBar);
    NEWUI_ELEMENT(Menu);
    NEWUI_ELEMENT(MenuItem);

    NEWUI_ELEMENT(Style);

    void Renderer::initContext() {
        context["ROM_MOUNT"] = ROM_MOUNT;
        context["MODULE_NAME"] = STR(MODULE_NAME_SPACES);

        env.add_callback("string", 1, [](const inja::Arguments& args) {
            return nlohmann::to_string(*args[0]);
        });

        env.add_callback("lua", 1, [](const inja::Arguments& args) {
            auto line = args[0]->get_ref<const std::string&>();

            char buffer[line.size() + 8];
            memset(buffer, 0, line.size() + 8);

            sprintf(buffer, "return %s", line.c_str());
            auto L = obtainLuaState();
            luaL_dostring(L, buffer);
            auto res = lua_table_to_json(L, 1);
            lua_pop(L, 1);
            return res;
        });
    }

    void Renderer::initTypes() {
        registerRenderer("", [](Renderer& renderer, newui::CachedRenderNode& node, nlohmann::json& context) {
            for (auto& child : node.children) {
                renderer.render(child, context);
            }
        });

#define _(Name) registerRenderer(STR(Name), Name)

        // Layout
        _(Root);
        _(Window);
        _(Columns);
        _(Row);
        _(Column);
        _(Child);
        _(CollapsingHeader);
        _(Group);

        // Content
        _(Include);
        _(Text);
        _(TextDisabled);
        _(Separator);
        _(SeparatorText);
        _(Spacing);
        _(Callback);

        // Controls
        _(Button);
        _(Combo);
        _(Selectable);
        _(Checkbox);
        _(InputInt);
        _(SliderInt);
        _(InputFloat);
        _(SliderFloat);

        // Menu
        _(MainMenuBar);
        _(MenuBar);
        _(Menu);
        _(MenuItem);

        // Style
        _(Style);

#undef _
    }

    void Renderer::initCallbacks() {
        registerCallback<void>(PLACEHOLDER, [](nlohmann::json& ctx){ if (jsonutil::getBool(ctx, "debug", false)) Logger::log("PLACEHOLDER"); });
        registerCallback<bool>(PLACEHOLDER, [](nlohmann::json& ctx, bool, bool){ if (jsonutil::getBool(ctx, "debug", false)) Logger::log("PLACEHOLDER"); });
        registerCallback<int>(PLACEHOLDER, [](nlohmann::json& ctx, int, int){ if (jsonutil::getBool(ctx, "debug", false)) Logger::log("PLACEHOLDER"); });
        registerCallback<float>(PLACEHOLDER, [](nlohmann::json& ctx, float, float){ if (jsonutil::getBool(ctx, "debug", false)) Logger::log("PLACEHOLDER"); });
        registerCallback<std::string&>(PLACEHOLDER, [](nlohmann::json& ctx, std::string&, std::string&){ if (jsonutil::getBool(ctx, "debug", false)) Logger::log("PLACEHOLDER"); });
    }

    Renderer::Renderer(Renderer* parent) : parent(parent) {
        initContext();
        initTypes();
        initCallbacks();
    }

    Renderer& Renderer::root() {
        auto current = this;
        while (current->parent != nullptr) {
            current = current->parent;
        }
        return *current;
    }

    void Renderer::registerRenderer(const std::string& type, RendererCallback callback) {
        root().renderers[type] = std::move(callback);
    }

    void Renderer::registerEnv(const std::function<void(inja::Environment&)>& callback) {
        callback(env);
    }

    void Renderer::registerLua(const std::function<void(lua_State*)>& callback) {
        callback(obtainLuaState());
    }

    void Renderer::init(const char* path) {
        auto fileResult = FileUtil::readFile(path);
        if (fileResult.has_value()) {
            auto& fileData = fileResult.value();
            uiTemplate = std::string(
                static_cast<const char*>(fileData.buffer.get()),
                fileData.size
            );
        } else {
            uiTemplate = std::format("<Text content=\"<% ERROR: Unable to read file '{}' %>\" />", path);
        }
        markNeedsUpdate();
    }

    void Renderer::render() {
        render(context);
    }

    bool Renderer::needsUpdate() {
        if (parent != nullptr && parent->needsUpdate()) {
            return true;
        }
        return m_needsUpdate;
    }

    void Renderer::markNeedsUpdate() {
        m_needsUpdate = true;
    }

    void Renderer::render(nlohmann::json& ctx) {
        if (needsUpdate()) {
            Logger::log("Updating Renderer[%p]!", this);
            auto templated = env.render(uiTemplate, ctx);

            auto result = doc.load_string(templated.c_str());
            if (result.status != pugi::status_ok) {
                Logger::log("XML: %s", templated.c_str());
                Logger::log("XML Parse Error at offset %d: %s", result.offset, result.description());
                uiTemplate = std::format("<TextDisabled content=\"XML Parse Error at offset {}: {}\" />", result.offset, result.description());
                return;
            }

            cachedRoot = std::move(buildCache(doc.root()));
            m_needsUpdate = false;
        }

        render(cachedRoot, ctx);
    }

    static void rendererFallbackFunc(Renderer&, CachedRenderNode& node, nlohmann::json& ctx) {
        ImGui::Text("<%% ERROR: Unknown node type '%s' at '%s' %%>", node.node.name(), node.node.path().c_str());
    }

    CachedRenderNode Renderer::buildCache(const pugi::xml_node& node) {
        auto type = substringAfter(const_cast<char*>(node.name()), ':');
        // Logger::log("Building %s", node.name());

        auto& rootRenderer = root();
        auto it = rootRenderer.renderers.find(type);
        RendererCallback cb;
        if (it == rootRenderer.renderers.end()) {
            cb = &rendererFallbackFunc;
        } else {
            cb = it->second;
        }

        std::vector<CachedRenderNode> children;
        for (auto& el : node.children()) {
            if (el.type() != pugi::node_element) continue;
            // Logger::log("Building child of %s: %s", node.name(), el.type());
            children.emplace_back(buildCache(el));
        }

        return {
            node,
            cb,
            std::move(children),
            {}
        };
    }

    void Renderer::render(CachedRenderNode& node, nlohmann::json& ctx) {
        ImGui::PushID(node.node.internal_object());
        node.cachedCallback(*this, node, ctx);
        ImGui::PopID();
    }
}
