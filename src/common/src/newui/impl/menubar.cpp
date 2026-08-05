#include "imgui.h"
#include "imgui_internal.h"
#include "newui/newui.h"
#include "util/InputUtil.h"

namespace newui {
    NEWUI_ELEMENT(MainMenuBar) {
        auto sticky = jsonutil::getBool(props, "sticky", false);
        if (!sticky && !InputUtil::isInputToggled()) return;

        if (ImGui::BeginMainMenuBar()) {
            for (const auto& child : node.children()) {
                renderer.render(child, context);
            }
            ImGui::EndMainMenuBar();
        }
    }

    NEWUI_ELEMENT(MenuBar) {
        if (ImGui::BeginMenuBar()) {
            for (const auto& child : node.children()) {
                renderer.render(child, context);
            }
            ImGui::EndMenuBar();
        }
    }

    NEWUI_ELEMENT(Menu) {
        auto label = props.value("label", "<% ERROR: Missing attribute 'label' %>");
        auto enabled = jsonutil::getBool(props, "enabled", true);

        if (ImGui::BeginMenu(label.c_str(), enabled)) {
            for (const auto& child : node.children()) {
                renderer.render(child, context);
            }
            ImGui::EndMenu();
        }
    }

    NEWUI_ELEMENT(MenuItem) {
        auto label = props.value("label", "<% ERROR: Missing attribute 'label' %>");
        auto icon = props.value("icon", "");
        auto iconStr = icon.empty() ? nullptr : icon.c_str();
        auto defaultValue = jsonutil::getBool(props, "default", false);
        auto key = props.value("key", "");
        auto enabled = jsonutil::getBool(props, "enabled", true);
        auto onChange = props.value("onChange", Renderer::PLACEHOLDER);

        if (!renderer.hasCallback<bool>(onChange)) {
            ImGui::BeginDisabled();
            auto content = std::format("<% ERROR: Unknown 'onchange' callback '{}' %>", onChange);
            ImGui::MenuItemEx(content.c_str(), iconStr, nullptr, false, enabled);
            ImGui::EndDisabled();
        }

        if (key.empty()) {
            ImGui::MenuItemEx("<% ERROR: Missing attribute 'key' %>", iconStr, nullptr, false, enabled);
        } else {
            auto current = jsonutil::getBool(context, key, defaultValue);
            if (ImGui::MenuItemEx(label.c_str(), iconStr, nullptr, current, enabled)) {
                renderer.invokeCallback(onChange, context, current, !current);
                context[key] = !current;
                renderer.markNeedsUpdate();
            }
        }
    }
}
