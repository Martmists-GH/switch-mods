#include <hook_util.h>
#include <hk/hook/Trampoline.h>

#include "externals/cmn/GameData.h"
#include "externals/gfl/string.h"
#include "externals/ik/BasePokemonParam.h"
#include "externals/ik/EventSystemCallFunctions.h"
#include "externals/pml/battle/Exp.h"
#include "externals/pml/Capture.h"
#include "externals/pml/pokepara/InitialSpec.h"
#include "externals/pml/pokepara/CalcTool.h"
#include "externals/ik/TrainerComponent.h"
#include "newui/jsonutil.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

// Settings
static bool isMustCapture = false;
static bool isExpShareOn = true;
static int expMultiplier = 1;
static bool expMultiplierInvert = false;
static bool alwaysMaxEnergy = false;
static bool movesAlwaysActive = false;

static PokemonData s_dataForEncounter = {};

extern "C" {
    void hook_setIsMustCapture(bool value) {
        isMustCapture = value;
    }

    void hook_setIsExpShareOn(bool value) {
        isExpShareOn = value;
    }

    void hook_setExpMultiplier(int value) {
        expMultiplier = value;
    }

    void hook_setExpMultiplierInvert(bool value) {
        expMultiplierInvert = value;
    }

    void hook_setAlwaysMaxEnergy(bool value) {
        alwaysMaxEnergy = value;
    }

    void hook_setMovesAlwaysActive(bool value) {
        movesAlwaysActive = value;
    }

    void hook_saveEncounterData(const nlohmann::json* data) {
        s_dataForEncounter.forceShiny = data->value("forceShiny", false);
        s_dataForEncounter.forceAlpha = data->value("forceAlpha", false);
        s_dataForEncounter.forceModify = data->value("forceModify", false);
        s_dataForEncounter.allowInvalidForms = data->value("allowInvalidForms", false);
        s_dataForEncounter.shinyMultiplier = data->value("shinyMultiplier", 1);
        s_dataForEncounter.species = data->value("species", 1);
        s_dataForEncounter.form = data->value("form", 0);
        s_dataForEncounter.sex = jsonutil::getInt(*data, "sex", 0);
        s_dataForEncounter.ability = data->value("ability", 0);
        s_dataForEncounter.nature = jsonutil::getInt(*data, "nature", 0);
        s_dataForEncounter.level = data->value("level", 1);
        s_dataForEncounter.iv[0] = data->value("iv0", 0);
        s_dataForEncounter.iv[1] = data->value("iv1", 0);
        s_dataForEncounter.iv[2] = data->value("iv2", 0);
        s_dataForEncounter.iv[3] = data->value("iv3", 0);
        s_dataForEncounter.iv[4] = data->value("iv4", 0);
        s_dataForEncounter.iv[5] = data->value("iv5", 0);
        s_dataForEncounter.ev[0] = data->value("ev0", 0);
        s_dataForEncounter.ev[1] = data->value("ev1", 0);
        s_dataForEncounter.ev[2] = data->value("ev2", 0);
        s_dataForEncounter.ev[3] = data->value("ev3", 0);
        s_dataForEncounter.ev[4] = data->value("ev4", 0);
        s_dataForEncounter.ev[5] = data->value("ev5", 0);
        s_dataForEncounter.moves[0] = data->value("move0", 1);
        s_dataForEncounter.moves[1] = data->value("move1", 2);
        s_dataForEncounter.moves[2] = data->value("move2", 3);
        s_dataForEncounter.moves[3] = data->value("move3", 4);
    }

    void hook_setWeather(const char* weather) {
        gfl::StringView holder = gfl::StringView::Create(weather);
        gfa::EventContext ctx = gfa::EventContext::make(&holder);
        ik::EventSystemCallFunctions::FixWeather(&ctx);
    }

    static nlohmann::json pokeData;
    nlohmann::json* hook_loadPartyData(int index) {
        auto party = cmn::GameData::GetPlayerParty();
        auto ptr = party.m_ptr->GetMemberPtr(index);

        if (ptr.m_ptr != nullptr) {
            auto pp = ptr.m_ptr->fields.m_pp->castTo<pml::pokepara::CoreParam>();
            auto acc = pp->fields.m_accessor;
            acc->StartFastMode();

            pokeData = {};
            pokeData["level"] = acc->calcData->level;

            acc->EndFastMode();
        } else {
            pokeData = nullptr;
        }

        return &pokeData;
    }

    void hook_savePartyData(int index, nlohmann::json* data) {
        auto party = cmn::GameData::GetPlayerParty();
        auto ptr = party.m_ptr->GetMemberPtr(index);

        if (ptr.m_ptr != nullptr && !pokeData.is_null()) {
            auto pp = ptr.m_ptr->fields.m_pp->castTo<pml::pokepara::CoreParam>();
            auto acc = pp->fields.m_accessor;
            acc->StartFastMode();

            acc->calcData->level = pokeData["level"];

            pp->UpdateCalcDatas(true);
            acc->EndFastMode();
        }
    }

    void hook_fixPartyData(int index) {
        auto party = cmn::GameData::GetPlayerParty();
        auto ptr = party.m_ptr->GetMemberPtr(index);

        if (ptr.m_ptr != nullptr && !pokeData.is_null()) {
            auto pp = ptr.m_ptr->fields.m_pp->castTo<pml::pokepara::CoreParam>();
            auto acc = pp->fields.m_accessor;
            acc->StartFastMode();
            if (acc->coreData->badEgg) {
                auto& b = acc->GetCoreDataBlockB();

                acc->coreData->badEgg = false;
                b.egg = false;
                pp->UpdateCalcDatas(true);
            }
            acc->EndFastMode();
        }
    }
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

HkTrampoline<void, pml::pokepara::InitialSpec*> ChangeEncounterHook = hk::hook::trampoline([](pml::pokepara::InitialSpec* spec) {
    spec->rareTryCount *= s_dataForEncounter.shinyMultiplier;
    ChangeEncounterHook.orig(spec);
    if (s_dataForEncounter.forceShiny) {
        spec->colorRnd = pml::pokepara::CalcTool::CorrectColorRndForRare(spec->id, spec->colorRnd);
    }
    if (s_dataForEncounter.forceAlpha) {
        spec->oybn = true;
        spec->attributeScaling = 0xFF;
        spec->ev[0] = 0xFC;
        spec->level = std::min(spec->level + 10, 100);  // This seems to be loaded from romfs, but I cba to write parsing for that mess
        for (int i = 0; i < 6; i++) {  // This is also loaded from romfs but uses this logic as fallback
            spec->iv[i] = 0x1F;
        }
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

HkTrampoline<void, ik::FieldPokemonParam*, int, int> SetMoveTimeoutHook = hk::hook::trampoline([](ik::FieldPokemonParam* param, int waza, int type) {
    if (!movesAlwaysActive) {
        SetMoveTimeoutHook.orig(param, waza, type);
    }
});

void battle_hooks() {
    CaptureHook.installAtPtr(pun<void*>(&pml::Capture::Judge));
    ExpCalcHook.installAtPtr(&pml::battle::Exp::CalcExp);
    ChangeEncounterHook.installAtPtr(pun<void*>(&pml::pokepara::InitialSpec::FixInitSpec));
    MegaEnergySetHook.installAtPtr(pun<void*>(&ik::TrainerComponent::SetMegaEnergy));
    SetMoveTimeoutHook.installAtPtr(pun<void*>(&ik::FieldPokemonParam::SetWazaRecastTime));
}
