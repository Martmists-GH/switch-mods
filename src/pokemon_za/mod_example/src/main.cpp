#include <hk/hook/Trampoline.h>
#include <logger/logger.h>
#include <nn/fs/fs_files.h>
#include <util/CodePatcher.h>
#include <util/FileUtil.h>
#include "game_constants.h"

#include "common_hooks.h"

HkTrampoline<void, void*, int> AnnounceHook = hk::hook::trampoline([](void* p1, int p2) {
    Logger::log("AnnounceHook: %p %d\n", p1, p2);
    ;
    AnnounceHook.orig(p1, p2);
});

void mod_init() {
    common_hooks();
    AnnounceHook.installAtMainOffset(0x00ab705c);
}
