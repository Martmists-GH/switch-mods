#pragma once

#include <vector>
#include <string>
#include <functional>
#include "ui/base/element.h"

namespace ui {
    ELEMENT(TextUnformatted) {
        std::string content;

        bool isValid()
        override{
                return !content.empty();
        }

        void draw()
        override{
                ImGui::TextUnformatted(content.c_str(), content.c_str() + content.size());
        }

        TextUnformatted() = default;
        explicit TextUnformatted(std::string content) : content(std::move(content)) {};
        explicit TextUnformatted(const char* content) : content(content) {};
    };
}
