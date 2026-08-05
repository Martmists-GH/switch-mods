#include "Renderer.h"

#include "game_constants.h"
#include "imgui.h"
#include "newui.h"
#include "hk/types.h"
#include "logger/logger.h"
#include "util/FileUtil.h"

namespace newui {
    NEWUI_ELEMENT(Root);
    NEWUI_ELEMENT(Window);
    NEWUI_ELEMENT(Row);
    NEWUI_ELEMENT(Column);
    NEWUI_ELEMENT(Child);
    NEWUI_ELEMENT(CollapsingHeader);

    NEWUI_ELEMENT(Include);
    NEWUI_ELEMENT(Text);
    NEWUI_ELEMENT(SeparatorText);
    NEWUI_ELEMENT(Spacing);

    NEWUI_ELEMENT(Button);
    NEWUI_ELEMENT(Selectable);
    NEWUI_ELEMENT(Checkbox);
    NEWUI_ELEMENT(InputInt);
    NEWUI_ELEMENT(InputFloat);

    NEWUI_ELEMENT(MainMenuBar);
    NEWUI_ELEMENT(MenuBar);
    NEWUI_ELEMENT(Menu);
    NEWUI_ELEMENT(MenuItem);

    void Renderer::initContext() {
        context["ROM_MOUNT"] = ROM_MOUNT;
        context["MODULE_NAME"] = STR(MODULE_NAME_SPACES);
    }

    void Renderer::initTypes() {
        // Layout
        registerRenderer("Root", Root);
        registerRenderer("Window", Window);
        registerRenderer("Row", Row);
        registerRenderer("Column", Column);
        registerRenderer("Child", Child);
        registerRenderer("CollapsingHeader", CollapsingHeader);

        // Content
        registerRenderer("Include", Include);
        registerRenderer("Text", Text);
        registerRenderer("SeparatorText", SeparatorText);
        registerRenderer("Spacing", Spacing);

        // Controls
        registerRenderer("Button", Button);
        registerRenderer("Selectable", Selectable);
        registerRenderer("Checkbox", Checkbox);
        registerRenderer("InputInt", InputInt);
        registerRenderer("InputFloat", InputFloat);

        // Menu
        registerRenderer("MainMenuBar", MainMenuBar);
        registerRenderer("MenuBar", MenuBar);
        registerRenderer("Menu", Menu);
        registerRenderer("MenuItem", MenuItem);
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
            auto templated = inja::render(uiTemplate, ctx);
            auto result = doc.load_string(templated.c_str());
            if (result.status != pugi::status_ok) {
                Logger::log("XML Parse Error: %s\n", result.description());
                uiTemplate = std::format("<Text content=\"XML Parse Error: {}\" />", result.description());
                return;
            }
            m_needsUpdate = false;
        }

        for (auto& node : doc.root().children()) {
            render(node, ctx);
        }
    }

    void Renderer::render(const pugi::xml_node& node, nlohmann::json& ctx) {
        if (node.type() == pugi::node_element) {
            auto& rootRenderer = root();

            if (rootRenderer.renderers.contains(node.name())) {
                ImGui::PushID(node.name());

                auto& renderer = rootRenderer.renderers[node.name()];
                auto props = parseAttrs(node);
                renderer(root(), node, props, ctx);

                ImGui::PopID();
            } else {
                ImGui::Text("<%% ERROR: Unknown node type '%s' at %s %%>", node.name(), node.path().c_str());
            }
        }
    }
}
