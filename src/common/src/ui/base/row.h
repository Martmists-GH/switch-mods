#pragma once

#include "ui/base/element.h"
#include "ui/base/button.h"
#include "ui/base/checkbox.h"
#include "ui/base/combo.h"
#include "ui/base/combo_simple.h"
#include "ui/base/input_int.h"
#include "ui/base/menu_bar.h"
#include "ui/base/function_wrapping_element.h"
#include "ui/base/row.h"
#include "ui/base/slider.h"
#include "ui/base/spacing.h"
#include "ui/base/text.h"
#include "ui/base/text_unformatted.h"
#include "ui/components/string_view.h"

namespace ui {
    ELEMENT(Row) {
        void draw()
        override{
            auto i = 0;
            for (auto child: mChildren) {
                if (i != 0) ImGui::SameLine();
                child->render();
                i++;
            }
        }

        COMMON_ELEMENTS();
    };
}
