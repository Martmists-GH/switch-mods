#pragma once

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

namespace ui {
    ELEMENT(Builder) {
        void render() override {
            if (canDisable) {
                ImGui::BeginDisabled(this->disabled);
            }
            if (this->beginDraw()) {
                this->draw();
                this->endDraw();
            }
            if (canDisable) {
                ImGui::EndDisabled();
            }
        }

        COMMON_ELEMENTS();
        ELEMENT_SUPPORTS_CHILD(CollapsingHeader);
        ELEMENT_SUPPORTS_CHILD(Row);
        ELEMENT_SUPPORTS_CHILD(Grid);
        ELEMENT_SUPPORTS_CHILD(Group);
        ELEMENT_SUPPORTS_CHILD(Child);
    };
}
