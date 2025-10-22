#pragma once

#include <string>
#include <functional>
#include "ui/base/element.h"

namespace ui {
    ELEMENT(Selectable) {
        std::string label;
        std::function<void()> onSelected;
        ImGuiSelectableFlags flags = ImGuiSelectableFlags_None;

        bool isValid()
        override{
                return !label.empty();
        }

        void draw()
        override{
                if (ImGui::Selectable(label.c_str(), false, flags)) {
                    if (onSelected) {
                        onSelected();
                    }
                }
        }

        Selectable() = default;
        explicit Selectable(std::string label, std::function<void()> callback) : label(std::move(label)), onSelected(std::move(callback)) {};
        explicit Selectable(std::string label, const ImGuiSelectableFlags flags, std::function<void()> callback) : label(std::move(label)), onSelected(std::move(callback)), flags(flags) {};
    };
}
