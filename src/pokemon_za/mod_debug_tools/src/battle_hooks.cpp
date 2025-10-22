#include <hk/hook/Trampoline.h>
#include "externals/pml/battle/Exp.h"
#include "externals/pml/Capture.h"

// Settings
static bool isMustCapture = false;
static bool isExpShareOn = true;
static int expMultiplier = 1;

void setIsMustCapture(bool value) {
    isMustCapture = value;
}

void setIsExpShareOn(bool value) {
    isExpShareOn = value;
}

void setExpMultiplier(int value) {
    expMultiplier = value;
}

HkTrampoline<void, pml::Capture*> CaptureHook = hk::hook::trampoline([](pml::Capture* param_1) {
    CaptureHook.orig(param_1);

    if (isMustCapture) {
        param_1->m_result.isCaptured = true;
        param_1->m_result.yureCount = 3;
        for (float & i : param_1->m_result.m_indicator) {
            i = 1.0;
        }
    }
});

HkTrampoline<void, pml::battle::Exp::CalcResult*, pml::battle::Exp::CalcParam*> ExpCalcHook = hk::hook::trampoline([](pml::battle::Exp::CalcResult* result, pml::battle::Exp::CalcParam* param) {
    ExpCalcHook.orig(result, param);
    if (!param->didFight && !isExpShareOn) {
        result->exp = 0;
    } else {
        result->exp *= expMultiplier;
    }
});

void battle_hooks() {
    CaptureHook.installAtPtr(pun<void*>(&pml::Capture::Judge));
    ExpCalcHook.installAtPtr(&pml::battle::Exp::CalcExp);
}
