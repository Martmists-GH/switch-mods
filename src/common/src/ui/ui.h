#pragma once

#include "ui/base/button.h"
#include "ui/base/checkbox.h"
#include "ui/base/child.h"
#include "ui/base/collapsing_header.h"
#include "ui/base/combo.h"
#include "ui/base/combo_simple.h"
#include "ui/base/group.h"
#include "ui/base/function_element.h"
#include "ui/base/input_int.h"
#include "ui/base/menu.h"
#include "ui/base/menu_bar.h"
#include "ui/base/menu_item.h"
#include "ui/base/root_element.h"
#include "ui/base/selectable.h"
#include "ui/base/slider.h"
#include "ui/base/spacing.h"
#include "ui/base/text.h"
#include "ui/base/text_unformatted.h"
#include "ui/base/window.h"
#include "ui/components/string_view.h"

ui::Root& getRootElement();

#define ROOT getRootElement()
