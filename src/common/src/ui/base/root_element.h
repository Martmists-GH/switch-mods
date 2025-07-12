#pragma once

#include <vector>
#include "ui/base/element.h"
#include "ui/base/window.h"
#include "imgui_backend/imgui_impl_nvn.hpp"
#include "checks.hpp"

namespace ui {
    ELEMENT(Root) {
        std::vector<std::function<void()>> callbacks = {};

        bool beginDraw() override {
            InputUtil::updatePadState();
            auto toggled = showing || InputUtil::isInputToggled();
            if (toggled) {
                showing = true;

                ImGuiIO& io = ImGui::GetIO();
                io.MouseDrawCursor = InputUtil::isInputToggled() && !is_emulator();
            }
            ImguiNvnBackend::newFrame();
            ImGui::NewFrame();
            return toggled;
        }

        void endDraw() override {
            ImGui::Render();
            ImguiNvnBackend::renderDrawData(ImGui::GetDrawData());
        }

        void render() override {
            if (canDisable) {
                ImGui::BeginDisabled(this->disabled);
            }
            if (this->beginDraw()) {
                this->draw();
            }
            for (auto& cb : callbacks) {
                cb();
            }
            this->endDraw();
            if (canDisable) {
                ImGui::EndDisabled();
            }
        }

        ELEMENT_SUPPORTS_CHILD(Window);

        private:
        bool showing = false;
    };
}
