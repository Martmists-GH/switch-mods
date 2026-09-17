#include "imgui.h"
#include "imgui_internal.h"
#include "newui/newui.h"
#include "util/InputUtil.h"

namespace newui {
    NEWUI_ELEMENT(MainMenuBar) {
        auto sticky = node.get<bool>("sticky", false);
        if (!sticky && !InputUtil::isInputToggled()) return;

        if (ImGui::BeginMainMenuBar()) {
            for (auto& child : node.children) {
                renderer.render(child, context);
            }
            ImGui::EndMainMenuBar();
        }
    }

    NEWUI_ELEMENT(MenuBar) {
        if (ImGui::BeginMenuBar()) {
            for (auto& child : node.children) {
                renderer.render(child, context);
            }
            ImGui::EndMenuBar();
        }
    }

    NEWUI_ELEMENT(Menu) {
        auto label = node.get<std::string_view>("label", "<% ERROR: Missing attribute 'label' %>");
        auto enabled = node.get<bool>("enabled", true);

        if (ImGui::BeginMenu(label.data(), enabled)) {
            for (auto& child : node.children) {
                renderer.render(child, context);
            }
            ImGui::EndMenu();
        }
    }

    NEWUI_ELEMENT(MenuItem) {
        auto label = node.get<std::string_view>("label", "<% ERROR: Missing attribute 'label' %>");
        auto icon = node.get<std::string_view>("icon", "");
        auto iconStr = icon.empty() ? nullptr : icon.data();
        auto defaultValue = node.get<bool>("default", false);
        auto key = node.get<std::string_view>("key", "");
        auto enabled = node.get<bool>("enabled", true);
        auto onChange = node.get<std::string_view>("onchange", Renderer::PLACEHOLDER);

        if (key.empty()) {
            ImGui::MenuItemEx("<% ERROR: Missing attribute 'key' %>", iconStr, nullptr, false, enabled);
        } else {
            auto current = jsonutil::getBool(context, key.data(), defaultValue);
            if (ImGui::MenuItemEx(label.data(), iconStr, nullptr, current, enabled)) {
                renderer.invokeCallback(onChange, "onchange", context, current, !current);
                jsonutil::assignPath(context, key, !current);
                renderer.markNeedsUpdate();
            }
        }
    }
}
