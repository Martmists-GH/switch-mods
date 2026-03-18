#include <hook_util.h>
#include <externals/btl/Exp.h>
#include <hk/hook/Trampoline.h>
#include "externals/btl/Capture.h"
// #include "externals/pml/pokepara/InitialSpec.h"
// #include "externals/pml/pokepara/CalcTool.h"
// #include "externals/ik/TrainerComponent.h"

// Settings
static bool isMustCapture = false;
static bool isExpShareOn = true;
static int expMultiplier = 1;
static bool expMultiplierInvert = false;

void setIsMustCapture(bool value) {
    isMustCapture = value;
}

void setIsExpShareOn(bool value) {
    isExpShareOn = value;
}

void setExpMultiplier(int value) {
    expMultiplier = value;
}

void setExpMultiplierInvert(bool value) {
    expMultiplierInvert = value;
}
PokemonData s_dataForEncounter = {};

HkTrampoline<void, btl::Capture::JudgeResult*, btl::Capture::JudgeParam*> CaptureHook = hk::hook::trampoline([](btl::Capture::JudgeResult* result, btl::Capture::JudgeParam* param) {
    CaptureHook.orig(result, param);

    if (isMustCapture) {
        result->isCaptured = true;
        result->yureCount = 3;
    }
});

HkTrampoline<void, btl::Exp::CalcParam*, btl::Exp::CalcResult*> ExpCalcHook = hk::hook::trampoline([](btl::Exp::CalcParam* param, btl::Exp::CalcResult* result) {
    ExpCalcHook.orig(param, result);
    if (!param->didFight && !isExpShareOn) {
        result->exp = 0;
    } else {
        if (expMultiplier == 0) {
            result->exp = 0;
        } else {
            if (expMultiplierInvert) {
                auto expf = (float)result->exp;
                result->exp = (int)(expf / (float)expMultiplier);
            } else {
                result->exp *= expMultiplier;
            }
        }
    }
});


void battle_hooks() {
    CaptureHook.installAtPtr(pun<void*>(&btl::Capture::JudgeCapture));
    ExpCalcHook.installAtPtr(&btl::Exp::CalcExp);
}
