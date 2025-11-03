#include <checks.hpp>
#include <externals/gfl/string.h>
#include <externals/pe/text/lua/Text.h>
#include <hk/hook/Trampoline.h>
#include <logger/logger.h>
#include <nlohmann/detail/value_t.hpp>
#include <nn/ro.h>
#include <util/InputUtil.h>

#include "common_hooks.h"
#include "mod_hooks.h"
#include "util/MessageUtil.h"

// nv::SetGraphicsAllocator(void* (*)(unsigned long, unsigned long, void*), void (*)(void*, void*), void* (*)(void*, unsigned long, void*), void*)
namespace {
    typedef void* (*AllocFunc)(ulong, ulong, void*);
    typedef void (*FreeFunc)(void*, void*);
    typedef void* (*ReallocFunc)(void*, ulong, void*);
}

static void* AllocProxy(ulong size, ulong align, void* ignore) {
    Logger::log("Alloc %ld %ld\n", size, align);
    return aligned_alloc(align, size);
}
static void FreeProxy(void* addr, void* ignore) {
    return free(addr);
}
static void* ReallocProxy(void* addr, ulong size, void* ignore) {
    return realloc(addr, size);
}

HkTrampoline<void, AllocFunc, FreeFunc, ReallocFunc, void*> SetGraphicsAllocHook = hk::hook::trampoline([](AllocFunc p1, FreeFunc p2, ReallocFunc p3, void* p4) {
    Logger::log("Swapping allocators!");
    SetGraphicsAllocHook.orig(p1, p2, ReallocProxy, p4);
});

HkTrampoline<void*, ulong, ulong, void*> AllocHook = hk::hook::trampoline([](ulong p1, ulong p2, void* p3) {
    return AllocHook.orig(p1, p2, p3);
});

HkTrampoline<void, void*, void*> FreeHook = hk::hook::trampoline([](void* p1, void* p2) {
    return FreeHook.orig(p1, p2);
});

HkTrampoline<void*, void*, ulong, void*> ReallocHook = hk::hook::trampoline([](void* p1, ulong p2, void* p3) {
    // if (p2 == 264 && InputUtil::isInputToggled()) {
    //     abort();
    //     // Logger::log("Allocating %ld bytes from gfx\n", p1);
    // }
    return ReallocHook.orig(p1, p2, p3);
});

// _ZN2nv20SetGraphicsAllocatorEPFPvmmS0_EPFvS0_S0_EPFS0_S0_mS0_ES0_

void mod_init() {
    if (get_host() == NXHost::YUZU) {
        MessageUtil::popup(0, "Detected Yuzu Emulator!", "This mod will not be disabled, but please note that no support will be offered.\nIf you are experiences issues with Yuzu, do not report the issue. Instead, try Ryujinx first, since it's a more accurate emulator.");
    }

    // uintptr_t addr;
    // nn::ro::LookupSymbol(&addr, "_ZN2nv20SetGraphicsAllocatorEPFPvmmS0_EPFvS0_S0_EPFS0_S0_mS0_ES0_");
    // SetGraphicsAllocHook.installAtMainOffset(0x02fc4fc0);
    // AllocHook.installAtMainOffset(0x01775980);
    // FreeHook.installAtMainOffset(0x017759e0);
    // ReallocHook.installAtMainOffset(0x01775a00);
    common_hooks();
    mod_hooks();
}
