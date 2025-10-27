#include <checks.hpp>

#include "common_hooks.h"
#include "mod_hooks.h"
#include "util/MessageUtil.h"

void mod_init() {
    if (get_host() == NXHost::YUZU) {
        MessageUtil::popup(0, "Detected Yuzu Emulator!", "This mod will not be disabled, but please note that no support will be offered.\nIf you are experiences issues with Yuzu, do not report the issue. Instead, try Ryujinx first, since it's a more accurate emulator.");
    }

    common_hooks();
    mod_hooks();
}
