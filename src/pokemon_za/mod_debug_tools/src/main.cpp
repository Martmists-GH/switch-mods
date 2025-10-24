#include "common_hooks.h"
#include "mod_hooks.h"

void mod_init() {
    common_hooks();
    mod_hooks();
}
