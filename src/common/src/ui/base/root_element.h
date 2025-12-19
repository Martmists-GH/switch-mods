#pragma once

#include <vector>
#include <nn/swkbd/swkbd.h>

#include "ui/base/element.h"
#include "ui/base/window.h"
#include "imgui_backend/imgui_impl_nvn.hpp"
#include "checks.hpp"

alignas(0x1000) static char workBuf[0x1000];
alignas(0x1000) static char mTextCheckBuf[0x7d4];
alignas(0x1000) static char mCustomizeDicBuf[0x400];

namespace ui {
    ELEMENT(Root) {
        std::vector<std::function<void()>> callbacks = {};
        bool swkbd_active = false;

        bool beginDraw() override {
            InputUtil::updatePadState();
            auto toggled = showing || InputUtil::isInputToggled();
            if (toggled) {
                showing = true;

                auto& io = ImGui::GetIO();
                io.MouseDrawCursor = InputUtil::isInputToggled() && !is_emulator();

                if (io.WantTextInput && !swkbd_active && !is_emulator()) {
                    swkbd_active = true;
                    auto state = ImGui::GetInputTextState(ImGui::GetActiveID());
                    nn::swkbd::ShowKeyboardArg arg{};
                    Logger::log("Size: 0x%x\n", sizeof(nn::swkbd::ShowKeyboardArg));
                    nn::swkbd::InitializeKeyboardConfig(&arg.keyboardConfig);
                    nn::swkbd::MakePreset(&arg.keyboardConfig, nn::swkbd::Preset::Default);
                    Logger::log("Required work buffer size: %x\n", nn::swkbd::GetRequiredWorkBufferSize(false));
                    Logger::log("Required text buffer size: %x\n", nn::swkbd::GetRequiredTextCheckWorkBufferSize());
                    memset(workBuf, 0, sizeof(workBuf));
                    memset(mTextCheckBuf, 0, sizeof(mTextCheckBuf));
                    memset(mCustomizeDicBuf, 0, sizeof(mCustomizeDicBuf));

                    arg.workBufSize = sizeof(workBuf);
                    arg.workBuf = workBuf;
                    arg.textCheckWorkBufSize = sizeof(mTextCheckBuf);
                    arg.textCheckWorkBuf = mTextCheckBuf;
                    arg._customizeDicBufSize = 0;
                    arg._customizeDicBuf = nullptr;
                    nn::swkbd::SetHeaderTextUtf8(&arg.keyboardConfig, "Input");
                    nn::swkbd::SetSubTextUtf8(&arg.keyboardConfig, "aaa");
                    arg.keyboardConfig.keyboardMode = nn::swkbd::KeyboardMode::ModeASCII;
                    arg.keyboardConfig.textMaxLength = 128;
                    arg.keyboardConfig.textMinLength = 1;
                    arg.keyboardConfig.isUseUtf8 = true;
                    arg.keyboardConfig.inputFormMode = nn::swkbd::InputFormMode::OneLine;
                    auto tmp = std::string(state->TextA.begin(), state->TextLen);
                    Logger::log("Initial text: %s\n", tmp.c_str());
                    nn::swkbd::SetInitialTextUtf8(&arg, tmp.c_str());
                    auto size = nn::swkbd::GetRequiredStringBufferSize();
                    Logger::log("String buffer size: %x\n", size);
                    auto buf = (char*)aligned_alloc(0x1000, size);
                    memset(buf, 0, size);

                    Logger::log("String buffer ptr: %p\n", buf);

                    nn::swkbd::String out(size, buf);
                    auto err = nn::swkbd::ShowKeyboard(&out, arg);
                    Logger::log("swkbd result: %d\n", err);
                    if (err != 671) {
                        Logger::log("Received text: %s\n", buf);
                        state->ClearText();
                        state->TextLen = 0;
                        io.AddInputCharactersUTF8(buf);
                    }
                    free(buf);
                } else if (!io.WantTextInput && swkbd_active) {
                    swkbd_active = false;
                }
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
