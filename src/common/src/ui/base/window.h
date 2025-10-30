#pragma once

#include <string>
#include "ui/base/element.h"
#include "ui/base/button.h"
#include "ui/base/checkbox.h"
#include "ui/base/child.h"
#include "ui/base/collapsing_header.h"
#include "ui/base/combo.h"
#include "ui/base/group.h"
#include "ui/base/input_int.h"
#include "ui/base/menu_bar.h"
#include "ui/base/row.h"
#include "ui/base/function_element.h"
#include "ui/base/function_wrapping_element.h"
#include "ui/base/grid.h"
#include "ui/base/slider.h"
#include "ui/base/spacing.h"
#include "ui/base/text.h"
#include "ui/components/string_view.h"
#include "util/InputUtil.h"

static ImVec2 gInvalidVec{-1, -1};

namespace ui {
    ELEMENT(Window) {
        std::string title;
        bool open = false;
        bool sticky = false;
        bool toggleable = true;
        bool _didBegin = false;
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
        ImVec2 initialSize = gInvalidVec;
        ImVec2 initialPos = gInvalidVec;

        bool isValid()
        override{
                return !title.empty();
        }

        void render()
        override{
                if (this->beginDraw()) {
                    this->draw();
                }
                if (_didBegin) {
                    this->endDraw();
                }
        }

        bool beginDraw()
        override{
                _didBegin = false;

                if (!sticky && !InputUtil::isInputToggled()) {
                    return false;
                }

                if (toggleable && !open) {
                    return false;
                }

                _didBegin = true;

                if (initialSize.x != gInvalidVec.x && initialSize.y != gInvalidVec.y) {
                    ImGui::SetNextWindowSize(initialSize, ImGuiCond_FirstUseEver);
                }

                if (initialPos.x != gInvalidVec.x && initialPos.y != gInvalidVec.y) {
                    ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);
                }

                if (toggleable) {
                    return ImGui::Begin(title.c_str(), &open, flags);
                } else {
                    return ImGui::Begin(title.c_str(), nullptr, flags);
                }
        }

        void endDraw()
        override{
                ImGui::End();
        }

        ELEMENT_SUPPORTS_CHILD(CollapsingHeader);
        ELEMENT_SUPPORTS_CHILD(Row);
        ELEMENT_SUPPORTS_CHILD(Grid);
        ELEMENT_SUPPORTS_CHILD(Group);
        ELEMENT_SUPPORTS_CHILD(Child);
        COMMON_ELEMENTS();
    };
}
