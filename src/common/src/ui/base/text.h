#pragma once

#include <utility>
#include <vector>
#include <string>
#include <functional>
#include "ui/base/element.h"

namespace ui {
    ELEMENT(Text) {
        std::string content;

        bool isValid()
        override{
                return !content.empty();
        }

        void draw()
        override{
                ImGui::Text("%s", content.c_str());
        }

        Text() = default;
        explicit Text(std::string content) : content(std::move(content)) {};
        explicit Text(const char* content) : content(content) {};
    };
}
