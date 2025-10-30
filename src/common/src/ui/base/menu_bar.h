#pragma once

#include <string>
#include "ui/base/element.h"

namespace ui {
    ELEMENT(MenuItem) {
        std::string label;
        const char *shortcut = nullptr;
        bool enabled = true;
        bool *checked;

        bool isValid()
        override{
            return !label.empty();
        }

        void draw()
        override{
            ImGui::MenuItem(label.c_str(), shortcut, checked, enabled);
        }
    };

    ELEMENT(Menu) {
        std::string label;
        bool enabled = true;

        bool isValid()
        override{
            return !label.empty();
        }

        bool beginDraw()
        override{
            return ImGui::BeginMenu(label.c_str(), enabled);
        }

        void endDraw()
        override{
            ImGui::EndMenu();
        }

        ELEMENT_SUPPORTS_CHILD(MenuItem)
    };

    ELEMENT(MenuBar) {
        bool beginDraw()
        override{
                return ImGui::BeginMenuBar();
        }

        void endDraw()
        override{
                return ImGui::EndMenuBar();
        }

        ELEMENT_SUPPORTS_CHILD(Menu)
    };
}
