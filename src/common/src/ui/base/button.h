#pragma once

#include <utility>
#include <vector>
#include <string>
#include <functional>
#include "ui/base/element.h"

namespace ui {
    ELEMENT(Button) {
        std::string label;
        std::function<void()> onClick;

        bool isValid()
        override{
                return !label.empty() && onClick;
        }

        void draw()
        override{
                if (ImGui::Button(label.c_str())) {
                    onClick();
                }
        }

        Button() = default;
        explicit Button(std::string label, std::function<void()> onClick) : label(std::move(label)), onClick(std::move(onClick)) {}
        explicit Button(const char* label, std::function<void()> onClick) : label(label), onClick(std::move(onClick)) {}
    };
}
