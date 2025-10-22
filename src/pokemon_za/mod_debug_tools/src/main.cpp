#include <hk/hook/Trampoline.h>
#include <logger/logger.h>
#include <nn/fs/fs_files.h>
#include <util/CodePatcher.h>
#include <util/FileUtil.h>
#include "game_constants.h"

#include "common_hooks.h"
#include "mod_hooks.h"

void mod_init() {
    common_hooks();
    mod_hooks();
}
