#include <checks.hpp>
#include <util/common_utils.h>

#include "common_hooks.h"
#include "mod_hooks.h"
#include "../../../../lib/std/llvm-project/libc/src/unistd/dup2.h"
#include "util/MessageUtil.h"

void mod_init() {
    common_hooks();
    mod_hooks();
}
