#pragma once

#include <utility>
#include <vector>
#include <string>
#include <functional>
#include "ui/base/element.h"

namespace ui {
    ELEMENT(Checkbox) {
        std::string label;
        bool enabled = false;
        std::function<void(bool)> onChange;

        bool isValid()
        override{
                return !label.empty();
        }

        void draw()
        override{
                if (ImGui::Checkbox(label.c_str(), &enabled)) {
                    if (onChange) {
                        onChange(enabled);
                    }
                }
        }

        Checkbox() = default;
        explicit Checkbox(std::string label) : label(std::move(label)) {};
        explicit Checkbox(const char* label) : label(label) {};
        explicit Checkbox(std::string label, std::function<void(bool)> onChange) : label(std::move(label)), onChange(std::move(onChange)) {};
        explicit Checkbox(const char* label, std::function<void(bool)> onChange) : label(label), onChange(std::move(onChange)) {};
        explicit Checkbox(std::string label, bool enabled, std::function<void(bool)> onChange) : label(std::move(label)), enabled(enabled), onChange(std::move(onChange)) {};
        explicit Checkbox(const char* label, bool enabled, std::function<void(bool)> onChange) : label(label), enabled(enabled), onChange(std::move(onChange)) {};
    };
}
