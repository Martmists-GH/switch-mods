#pragma once
#include <checks.hpp>
#include <strings.hpp>
#include "config.h"
#include <vector>
#include "logger/logger.h"

std::vector<std::string> getEventList();
void triggerEvent(const std::string& eventName);

#define CC_TOGGLE(name) bool name##Active; uint64_t name##Timer
#define CC_ENABLE(name) flags.name##Active = true; flags.name##Timer = (nn::os::ConvertToTimeSpan(nn::os::GetSystemTick()).nanoseconds / (1000 * 1000 * 1000)) + getConfig().effectDuration
#define CC_DISABLE(name) flags.name##Active = false
#define CC_CHECK(name) if (flags.name##Active && flags.name##Timer < (nn::os::ConvertToTimeSpan(nn::os::GetSystemTick()).nanoseconds / (1000 * 1000 * 1000))) { flags.name##Active = false; } else if (flags.name##Active)
#define CC_CHECK_NOT(name) if (flags.name##Active && flags.name##Timer < (nn::os::ConvertToTimeSpan(nn::os::GetSystemTick()).nanoseconds / (1000 * 1000 * 1000)))

struct CC_flags {
    CC_TOGGLE(expGain1k);
    CC_TOGGLE(expGainZero);
    CC_TOGGLE(neverCapture);
    CC_TOGGLE(alwaysCapture);
    CC_TOGGLE(invertTypeEffects);
    CC_TOGGLE(disableTypeEffects);
    CC_TOGGLE(randomTypeEffects);
    CC_TOGGLE(randomFilter);
    CC_TOGGLE(invertSTAB);
    CC_TOGGLE(disableRideAbilities);
    CC_TOGGLE(randomWildSpawns);
    CC_TOGGLE(increasedShinyOdds);
    CC_TOGGLE(wildSpawnBoost);
};
CC_flags& getFlags();
void cc_hooks();

enum CC_lang {
    EN,
    ES,
};

static CC_lang CC_lang_current() {
    uint8_t langId;
    if (is_version("3.0.1")) {
        langId = *(uint8_t*)(((uintptr_t*)(hk::ro::getMainModule()->range().start() + 0x04735140)) + 0x2c);
    } else {
        HK_ABORT("CC_lang_current not implemented for version!", nullptr);
    }

    switch (langId) {
        // case 1: return CC_lang::JP;
    case 2: return CC_lang::EN;
        // case 3: return CC_lang::FR;
        // case 4: return CC_lang::IT;
        // case 5: return CC_lang::DE;
        // case 6: return CC_lang::???;
    case 7: return CC_lang::ES;
        // case 8: return CC_lang::KO;
        // case 9: return CC_lang::CH_SIMP;
        // case 10: return CC_lang::CH_TRAD;
    }
    return CC_lang::EN;
}

static strings_crowdcontrol_t& getStrings(CC_lang lang) {
    switch (lang) {
        case CC_lang::EN: return english.crowdcontrol;
        case CC_lang::ES: return spanish.crowdcontrol;
    }
    Logger::log("Invalid language: %d\n", lang);
    return english.crowdcontrol;
}
