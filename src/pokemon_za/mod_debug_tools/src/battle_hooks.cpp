#include <hook_util.h>
#include <hk/hook/Trampoline.h>
#include "externals/pml/battle/Exp.h"
#include "externals/pml/Capture.h"
#include "externals/pml/pokepara/InitialSpec.h"
#include "externals/pml/pokepara/CalcTool.h"
#include "externals/ik/TrainerComponent.h"

// Settings
static bool isMustCapture = false;
static bool isExpShareOn = true;
static int expMultiplier = 1;
static bool expMultiplierInvert = false;
static bool alwaysMaxEnergy = false;

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

void setAlwaysMaxEnergy(bool value) {
    alwaysMaxEnergy = value;
}

PokemonData s_dataForEncounter = {};

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

HkTrampoline<void, pml::pokepara::InitialSpec*> ForceShinyHook = hk::hook::trampoline([](pml::pokepara::InitialSpec* spec) {
    spec->rareTryCount *= s_dataForEncounter.shinyMultiplier;
    ForceShinyHook.orig(spec);
    if (s_dataForEncounter.forceShiny) {
        spec->colorRnd = pml::pokepara::CalcTool::CorrectColorRndForRare(spec->id, spec->colorRnd);
    }
    if (s_dataForEncounter.forceAlpha) {
        spec->oybn = true;
    }
    if (s_dataForEncounter.forceModify) {
        spec->monsNo = s_dataForEncounter.species;
        spec->formNo = s_dataForEncounter.form;
        spec->level = s_dataForEncounter.level;
        spec->sex = s_dataForEncounter.sex;
        spec->nature = s_dataForEncounter.nature;
        spec->natureMint = s_dataForEncounter.nature;
        spec->abilityIndex = s_dataForEncounter.ability;
        for (int i = 0; i < 6; i++) {
            spec->iv[i] = s_dataForEncounter.iv[i];
            spec->ev[i] = s_dataForEncounter.ev[i];
        }
    }
});

HkTrampoline<void, ik::TrainerComponent*, float> MegaEnergySetHook = hk::hook::trampoline([](ik::TrainerComponent* param_1, float param_2){
    if (alwaysMaxEnergy) {
        MegaEnergySetHook.orig(param_1, param_1->m_megaEnergyMax);
    } else {
        MegaEnergySetHook.orig(param_1, param_2);
    }
});

void battle_hooks() {
    CaptureHook.installAtPtr(pun<void*>(&pml::Capture::Judge));
    ExpCalcHook.installAtPtr(&pml::battle::Exp::CalcExp);
    ForceShinyHook.installAtPtr(pun<void*>(&pml::pokepara::InitialSpec::FixInitSpec));
    MegaEnergySetHook.installAtPtr(pun<void*>(&ik::TrainerComponent::SetMegaEnergy));
}
