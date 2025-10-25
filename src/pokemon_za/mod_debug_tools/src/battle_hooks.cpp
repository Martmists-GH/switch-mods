#include <hk/hook/Trampoline.h>
#include "externals/pml/battle/Exp.h"
#include "externals/pml/Capture.h"
#include "externals/pml/pokepara/InitialSpec.h"
#include "externals/pml/pokepara/CalcTool.h"

// Settings
static bool isMustCapture = false;
static bool isExpShareOn = true;
static int expMultiplier = 1;
static bool expMultiplierInvert = false;
static bool isMustShiny = false;
static int shinyMultiplier = 1;

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

void setIsMustShiny(bool value) {
    isMustShiny = value;
}

void setShinyMultiplier(int value) {
    shinyMultiplier = value;
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

HkTrampoline<void, pml::pokepara::InitialSpec*> ForceShinyHook = hk::hook::trampoline([](pml::pokepara::InitialSpec* param_1) {
    param_1->rareTryCount *= shinyMultiplier;
    ForceShinyHook.orig(param_1);
    if (isMustShiny) {
        param_1->colorRnd = pml::pokepara::CalcTool::CorrectColorRndForRare(param_1->id, param_1->colorRnd);
    }
});

void battle_hooks() {
    CaptureHook.installAtPtr(pun<void*>(&pml::Capture::Judge));
    ExpCalcHook.installAtPtr(&pml::battle::Exp::CalcExp);
    ForceShinyHook.installAtPtr(pun<void*>(&pml::pokepara::InitialSpec::FixInitSpec));
}
