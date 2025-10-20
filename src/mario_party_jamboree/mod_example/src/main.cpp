#include <hk/hook/Trampoline.h>
#include <logger/logger.h>
#include <nn/ro.h>

#include "externals/bq/BoardWork.h"

HkTrampoline<void, bq::BoardWork*, int> BoardWorkMaxTurnHook = hk::hook::trampoline([](bq::BoardWork* board, int maxTurn){
    BoardWorkMaxTurnHook.orig(board, 127);
});

HkTrampoline<void> DisableGameSocket = hk::hook::trampoline([]() {

});

extern "C" void nnsocketInitializeStub();

static int counter = 0;
int getCounter() { return counter; }

HkTrampoline<nn::Result, nn::ro::Module*, const void*, void*, unsigned long, int, bool> LoadModuleHook = hk::hook::trampoline([](nn::ro::Module* param_1, const void* param_2, void* param_3, unsigned long param_4, int param_5, bool param_6) {
    // counter++;
    return LoadModuleHook.orig(param_1, param_2, param_3, param_4, param_5, param_6);
});

HkTrampoline<nn::Result, nn::ro::Module*> UnloadModuleHook = hk::hook::trampoline([](nn::ro::Module* param_1) {
    // counter--;
    return UnloadModuleHook.orig(param_1);
});


void mod_init() {
    BoardWorkMaxTurnHook.installAtPtr(pun<void*>(&bq::BoardWork::SetTurnMax));
    //DisableGameSocket.installAtPtr(&nnsocketInitializeStub);

    LoadModuleHook.installAtPtr(static_cast<nn::Result(*)(nn::ro::Module*, const void*, void*, u64, s32, bool)>(&nn::ro::LoadModule));
    UnloadModuleHook.installAtPtr(&nn::ro::UnloadModule);
}
