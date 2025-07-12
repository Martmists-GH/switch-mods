#include <cc_hooks.h>
#include <common_hooks.h>

void mod_init() {
    common_hooks();
    cc_hooks();
}
