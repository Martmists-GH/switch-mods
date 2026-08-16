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
        auto label = jsonutil::getString(props, "label", "<% ERROR: Missing attribute 'label' %>");
        auto enabled = jsonutil::getBool(props, "enabled", true);

        if (ImGui::BeginMenu(label.data(), enabled)) {
            for (const auto& child : node.children()) {
                renderer.render(child, context);
            }
            ImGui::EndMenu();
        }
    }

    NEWUI_ELEMENT(MenuItem) {
        auto label = jsonutil::getString(props, "label", "<% ERROR: Missing attribute 'label' %>");
        auto icon = jsonutil::getString(props, "icon", "");
        auto iconStr = icon.empty() ? nullptr : icon.data();
        auto defaultValue = jsonutil::getBool(props, "default", false);
        auto key = jsonutil::getString(props, "key", "");
        auto enabled = jsonutil::getBool(props, "enabled", true);
        auto onChange = jsonutil::getString(props, "onchange", Renderer::PLACEHOLDER);

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
