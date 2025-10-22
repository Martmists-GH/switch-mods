#pragma once

#include "ui/base/element.h"
#include "ui/base/button.h"
#include "ui/base/checkbox.h"
#include "ui/base/combo.h"
#include "ui/base/combo_simple.h"
#include "ui/base/group.h"
#include "ui/base/input_int.h"
#include "ui/base/menu_bar.h"
#include "ui/base/slider.h"
#include "ui/base/spacing.h"
#include "ui/base/text.h"
#include "ui/base/text_unformatted.h"
#include "ui/components/string_view.h"

namespace ui {
    ELEMENT(FunctionWrappingElement) {
        std::function<bool()> callbackBefore;
        std::function<void()> callbackAfter;

        bool isValid()
        override{
            return callbackBefore != nullptr && callbackAfter != nullptr;
        }

        bool beginDraw() override {
            return callbackBefore();
        }

        void endDraw() override {
            return callbackAfter();
        }

        ELEMENT_SUPPORTS_CHILD(Button)
        ELEMENT_SUPPORTS_CHILD(Checkbox)
        ELEMENT_SUPPORTS_CHILD(Combo)
        ELEMENT_SUPPORTS_CHILD(ComboSimple)
        ELEMENT_SUPPORTS_CHILD(FunctionElement)
        ELEMENT_SUPPORTS_CHILD(InputInt)
        ELEMENT_SUPPORTS_CHILD(MenuBar)
        ELEMENT_SUPPORTS_CHILD(SliderFloat)
        ELEMENT_SUPPORTS_CHILD(SliderInt)
        ELEMENT_SUPPORTS_CHILD(Spacing)
        ELEMENT_SUPPORTS_CHILD(Text)
        ELEMENT_SUPPORTS_CHILD(TextUnformatted)
        ELEMENT_SUPPORTS_CHILD(StringView)
    };
}
