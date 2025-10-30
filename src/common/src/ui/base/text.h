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

    ELEMENT(TextSeparator) {
        std::string content;

        bool isValid()
        override{
            return !content.empty();
        }

        void draw()
        override{
            ImGui::SeparatorText(content.c_str());
        }

        TextSeparator() = default;
        explicit TextSeparator(std::string content) : content(std::move(content)) {};
        explicit TextSeparator(const char* content) : content(content) {};
    };
}
