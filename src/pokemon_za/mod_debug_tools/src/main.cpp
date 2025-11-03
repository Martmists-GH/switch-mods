#include <checks.hpp>
#include <externals/gfl/string.h>
#include <externals/pe/text/lua/Text.h>
#include <hk/hook/Trampoline.h>
#include <logger/logger.h>
#include <util/InputUtil.h>

#include "common_hooks.h"
#include "mod_hooks.h"
#include "util/MessageUtil.h"


HkTrampoline<void*, void*, size_t, size_t, size_t> AllocHook = hk::hook::trampoline([](void* p1, size_t p2, size_t p3, size_t p4) {
    gfl::SizedHeap** p = (gfl::SizedHeap**)p1;
    auto heap = p[1];
    if (strcmp(heap->impl()->fields.label, "TrinityHeapType::Gfx2") == 0) {
        if (InputUtil::isInputToggled()) {
            Logger::log("Allocating on GFX2");
        }
    }
    return AllocHook.orig(p1, p2, p3, p4);
});

void mod_init() {
    if (get_host() == NXHost::YUZU) {
        MessageUtil::popup(0, "Detected Yuzu Emulator!", "This mod will not be disabled, but please note that no support will be offered.\nIf you are experiences issues with Yuzu, do not report the issue. Instead, try Ryujinx first, since it's a more accurate emulator.");
    }

    AllocHook.installAtMainOffset(0x00000028);
    common_hooks();
    mod_hooks();
}
