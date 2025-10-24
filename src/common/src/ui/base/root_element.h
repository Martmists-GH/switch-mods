#pragma once

#include <vector>
#include <nn/swkbd/swkbd.h>

#include "ui/base/element.h"
#include "ui/base/window.h"
#include "imgui_backend/imgui_impl_nvn.hpp"
#include "checks.hpp"

// static char workBuf[0x2000];
// static char mTextCheckBuf[0x400];
// static char mCustomizeDicBuf[0x400];

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

                // FIXME: Crashes in nn::swkbd::ShowKeyboard at nn::applet::CreateLargeStorage
                // if (io.WantTextInput && !is_emulator()) {
                //     auto state = ImGui::GetInputTextState(ImGui::GetActiveID());
                //     nn::swkbd::ShowKeyboardArg arg{};
                //     nn::swkbd::MakePreset(&arg.keyboardConfig, nn::swkbd::Preset::Default);
                //     arg.workBufSize = sizeof(workBuf);
                //     arg.workBuf = workBuf;
                //     arg.textCheckWorkBufSize = sizeof(mTextCheckBuf);
                //     arg.textCheckWorkBuf = mTextCheckBuf;
                //     arg._customizeDicBufSize = sizeof(mCustomizeDicBuf);
                //     arg._customizeDicBuf = mCustomizeDicBuf;
                //     nn::swkbd::SetHeaderTextUtf8(&arg.keyboardConfig, "Input");
                //     nn::swkbd::SetSubTextUtf8(&arg.keyboardConfig, "aaa");
                //     arg.keyboardConfig.keyboardMode = nn::swkbd::KeyboardMode::ModeASCII;
                //     arg.keyboardConfig.isUseUtf8 = true;
                //     arg.keyboardConfig.textMaxLength = 128;
                //     arg.keyboardConfig.textMinLength = 1;
                //     arg.keyboardConfig.isUseUtf8 = true;
                //     arg.keyboardConfig.inputFormMode = nn::swkbd::InputFormMode::OneLine;
                //     auto tmp = std::string(state->TextA.begin(), state->TextLen);
                //     Logger::log("Initial text: %s\n", tmp.c_str());
                //     // nn::swkbd::SetInitialTextUtf8(&arg, tmp.c_str());
                //     auto size = nn::swkbd::GetRequiredStringBufferSize();
                //     auto buf = (char*)malloc(size);
                //     nn::swkbd::String out(size, buf);
                //     if (nn::swkbd::ShowKeyboard(&out, arg) != 671) {
                //         state->ClearText();
                //         state->TextLen = 0;
                //         io.AddInputCharactersUTF8(buf);
                //     }
                //     free(buf);
                // }
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
