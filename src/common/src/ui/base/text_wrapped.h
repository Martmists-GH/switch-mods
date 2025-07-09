#pragma once

#include <vector>
#include <string>
#include <functional>
#include "ui/base/element.h"

namespace ui {
    ELEMENT(TextWrapped) {
        std::string content;

        bool isValid()
        override{
                return !content.empty();
        }

        void draw()
        override{
                ImGui::TextWrapped("%s", content.c_str());
        }
    };
}
