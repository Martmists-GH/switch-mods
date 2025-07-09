#pragma once

#include <vector>
#include "ui/base/element.h"
#include "ui/base/window.h"
#include "imgui_backend/imgui_impl_nvn.hpp"
#include "checks.hpp"

namespace ui {
    ELEMENT(Root) {
        bool beginDraw() override {
                InputUtil::updatePadState();
                if (showing || InputUtil::isInputToggled()) {
                    showing = true;

                    ImGuiIO& io = ImGui::GetIO();
                    io.MouseDrawCursor = InputUtil::isInputToggled() && !is_emulator();

                    ImguiNvnBackend::newFrame();
                    ImGui::NewFrame();
                    return true;
                }
                return false;
        }

        void endDraw() override {
                ImGui::Render();
                ImguiNvnBackend::renderDrawData(ImGui::GetDrawData());
        }

        ELEMENT_SUPPORTS_CHILD(Window);

        private:
        bool showing = false;
    };
}
