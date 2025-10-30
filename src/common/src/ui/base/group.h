#pragma once

#include <imgui_internal.h>

#include "ui/base/element.h"
#include "ui/base/button.h"
#include "ui/base/checkbox.h"
#include "ui/base/combo.h"
#include "ui/base/group.h"
#include "ui/base/input_int.h"
#include "ui/base/menu_bar.h"
#include "ui/base/function_wrapping_element.h"
#include "ui/base/slider.h"
#include "ui/base/spacing.h"
#include "ui/base/text.h"
#include "ui/components/string_view.h"

namespace ui {
    ELEMENT(Group) {
        bool beginDraw() override {
            ImGui::BeginGroup();
            return true;
        }

        void endDraw() override {
            ImGui::EndGroup();
        }

        COMMON_ELEMENTS();
    };
}
