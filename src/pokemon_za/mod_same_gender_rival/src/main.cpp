#include <hk/hook/Trampoline.h>
#include <logger/logger.h>
#include <nn/fs/fs_files.h>
#include <util/CodePatcher.h>
#include <util/FileUtil.h>
#include "game_constants.h"

#include "common_hooks.h"

static bool s_shouldPatch = true;
void setGenderModShouldPatch(bool shouldPatch) {
    s_shouldPatch = shouldPatch;
}

HkTrampoline<bool, void*> GenderPatchHookForModel = hk::hook::trampoline([](void* param_1) {
    const auto r = GenderPatchHookForModel.orig(param_1);
    if (s_shouldPatch) {
        return !r;
    } else {
        return r;
    }
});

void mod_init() {
    common_hooks();
    GenderPatchHookForModel.installAtMainOffset(0x00462620);
}
