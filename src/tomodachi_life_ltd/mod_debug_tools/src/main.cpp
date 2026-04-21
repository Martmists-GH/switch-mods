#include "externals/sead/Heap.h"
#include "hk/hook/Trampoline.h"

static bool didInit = false;
HkTrampoline<void> initRoot = hk::hook::trampoline([]() {

});

void mod_init() {

}
