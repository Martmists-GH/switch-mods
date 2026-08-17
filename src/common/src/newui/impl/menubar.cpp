#include "imgui.h"
#include "imgui_internal.h"
#include "newui/newui.h"
#include "util/InputUtil.h"

namespace newui {
    NEWUI_ELEMENT(MainMenuBar) {
        auto sticky = jsonutil::getBool(node, "sticky", false);
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
        auto label = jsonutil::getString(node, "label", "<% ERROR: Missing attribute 'label' %>");
        auto enabled = jsonutil::getBool(node, "enabled", true);

        if (ImGui::BeginMenu(label.data(), enabled)) {
            for (const auto& child : node.children()) {
                renderer.render(child, context);
            }
            ImGui::EndMenu();
        }
    }

    NEWUI_ELEMENT(MenuItem) {
        auto label = jsonutil::getString(node, "label", "<% ERROR: Missing attribute 'label' %>");
        auto icon = jsonutil::getString(node, "icon", "");
        auto iconStr = icon.empty() ? nullptr : icon.data();
        auto defaultValue = jsonutil::getBool(node, "default", false);
        auto key = jsonutil::getString(node, "key", "");
        auto enabled = jsonutil::getBool(node, "enabled", true);
        auto onChange = jsonutil::getString(node, "onchange", Renderer::PLACEHOLDER);

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
