#include <random>
#include <hk/types.h>
#include <hk/ro/RoUtil.h>
#include <logger/logger.h>
#include <net/WebsocketClient.h>
#include "util/CodePatcher.h"
#include <nn/os.h>
#include <lua_utils.h>
#include <externals/btl/type_matchups.h>
#include <externals/btl/Capture.h>
#include <externals/btl/EventLauncher.h>
#include <externals/btl/Exp.h>
#include <hk/hook/Trampoline.h>

#include "cc_hooks.h"

static CC_flags flags;
CC_flags& getFlags() { return flags; }

#define x0 0
#define xH 2
#define x1 4
#define x2 8
static const uint8_t TYPE_EFFECT_TBL[18][18] = {
    // Vertical is attacker type, horizontal is defender type. For example, 1 down from top left is Fighting attacker, normal defender.
    // Order (L->R and T->B): Normal, Fighting, Flying, Poison, Ground, Rock, Bug, Ghost, Steel, Fire, Water, Grass, Electric, Psychic, Ice, Dragon, Dark, Fairy
    {x1,x1,x1,x1,x1,xH,x1,x0,xH,x1,x1,x1,x1,x1,x1,x1,x1,x1},
    {x2,x1,xH,xH,x1,x2,xH,x0,x2,x1,x1,x1,x1,xH,x2,x1,x2,xH},
    {x1,x2,x1,x1,x1,xH,x2,x1,xH,x1,x1,x2,xH,x1,x1,x1,x1,x1},
    {x1,x1,x1,xH,xH,xH,x1,xH,x0,x1,x1,x2,x1,x1,x1,x1,x1,x2},
    {x1,x1,x0,x2,x1,x2,xH,x1,x2,x2,x1,xH,x2,x1,x1,x1,x1,x1},
    {x1,xH,x2,x1,xH,x1,x2,x1,xH,x2,x1,x1,x1,x1,x2,x1,x1,x1},
    {x1,xH,xH,xH,x1,x1,x1,xH,xH,xH,x1,x2,x1,x2,x1,x1,x2,xH},
    {x0,x1,x1,x1,x1,x1,x1,x2,x1,x1,x1,x1,x1,x2,x1,x1,xH,x1},
    {x1,x1,x1,x1,x1,x2,x1,x1,xH,xH,xH,x1,xH,x1,x2,x1,x1,x2},
    {x1,x1,x1,x1,x1,xH,x2,x1,x2,xH,xH,x2,x1,x1,x2,xH,x1,x1},
    {x1,x1,x1,x1,x2,x2,x1,x1,x1,x2,xH,xH,x1,x1,x1,xH,x1,x1},
    {x1,x1,xH,xH,x2,x2,xH,x1,xH,xH,x2,xH,x1,x1,x1,xH,x1,x1},
    {x1,x1,x2,x1,x0,x1,x1,x1,x1,x1,x2,xH,xH,x1,x1,xH,x1,x1},
    {x1,x2,x1,x2,x1,x1,x1,x1,xH,x1,x1,x1,x1,xH,x1,x1,x0,x1},
    {x1,x1,x2,x1,x2,x1,x1,x1,xH,xH,xH,x2,x1,x1,xH,x2,x1,x1},
    {x1,x1,x1,x1,x1,x1,x1,x1,xH,x1,x1,x1,x1,x1,x1,x2,x1,x0},
    {x1,xH,x1,x1,x1,x1,x1,x2,x1,x1,x1,x1,x1,x2,x1,x1,xH,xH},
    {x1,x2,x1,xH,x1,x1,x1,x1,xH,xH,x1,x1,x1,x1,x1,x2,x2,x1},
};

typedef uint8_t type_arr[18][18];
static std::map<const char*, std::function<void()>>& allEvents() {
    auto lang = CC_lang_current();
    auto strings = getStrings(lang);

    static std::map<const char*, std::function<void()>> eventsList = {
        // Effects with duration
        {strings.event.exp_x1000, [] { CC_ENABLE(expGain1k); }},
        {strings.event.exp_x0, [] { CC_ENABLE(expGainZero); }},
        {strings.event.catch_rate_zero, [] { CC_ENABLE(neverCapture); }},
        {strings.event.catch_rate_always, [] { CC_ENABLE(alwaysCapture); }},
        {strings.event.types_invert, [] {
            CC_ENABLE(invertTypeEffects);
            CodePatcher cp(&btl::TYPE_EFFECT_TBL);

            Logger::log("Creating patched table\n");
            for (int i = 0; i < 18; i++) {
                for (int j = 0; j < 18; j++) {
                    cp.write(TYPE_EFFECT_TBL[j][i]);
                }
            }
        }},
        {strings.event.types_none, [] {
            CC_ENABLE(disableTypeEffects);
            CodePatcher cp(&btl::TYPE_EFFECT_TBL);
            for (int i = 0; i < 18; i++) {
                for (int j = 0; j < 18; j++) {
                    cp.write(static_cast<uint8_t>(2));
                }
            }
        }},
        {strings.event.types_random, [] {
            CC_ENABLE(randomTypeEffects);
            CodePatcher cp(&btl::TYPE_EFFECT_TBL);
            std::mt19937 rng(nn::os::GetSystemTick().value);
            auto dist = std::uniform_int_distribution(0, 323);

            for (int i = 0; i < 18; i++) {
                for (int j = 0; j < 18; j++) {
                    const auto r = dist(rng);
                    uint8_t out;

                    if (r < 8) {
                        out = x0;
                    } else if (r < 8 + 51) {
                        out = x2;
                    } else if (r < 8 + 51 + 61) {
                        out = xH;
                    } else {
                        out = x1;
                    }

                    cp.write(out);
                }
            }
        }},
        {strings.event.filter_random, [] {
            CC_ENABLE(randomFilter);
            const std::string tmp = std::string("API:CC_RANDOM_FILTER(") + std::to_string(getConfig().effectDuration) + ")";
            executeLua(get_lua_state(), tmp.c_str(), "<CC:Apply a random filter>");
        }},
        {strings.event.stab_invert, [] { CC_ENABLE(invertSTAB); }},
        {strings.event.ride_disable, [] {
            CC_ENABLE(disableRideAbilities);
            const std::string tmp = std::string("API:CC_DISABLE_ABILITIES(") + std::to_string(getConfig().effectDuration) + ")";
            executeLua(get_lua_state(), tmp.c_str(), "<CC:Disable ride abilities>");
        }},
        {strings.event.wild_random, [] {
            CC_ENABLE(randomWildSpawns);
            const std::string tmp = std::string("API:CC_ENABLE_RANDOM_SPAWNS(") + std::to_string(getConfig().effectDuration) + ")";
            executeLua(get_lua_state(), tmp.c_str(), "<CC:Randomize wild pokemon>");
        }},
        {strings.event.wild_increase, [] {
            CC_ENABLE(wildSpawnBoost);
            const std::string tmp = std::string("API:CC_ENABLE_SPAWN_BOOST(") + std::to_string(getConfig().effectDuration) + ")";
            executeLua(get_lua_state(), tmp.c_str(), "<CC:10x more wild pokemon>");
        }},
        {strings.event.shiny_increase, [] {
            CC_ENABLE(increasedShinyOdds);
            const std::string tmp = std::string("API:CC_ENABLE_SHINY_BOOST(") + std::to_string(getConfig().effectDuration) + ")";
            executeLua(get_lua_state(), tmp.c_str(), "<CC:Shiny season>");
        }},

        // Triggers
        {strings.event.spawn_lv90, [] {
            executeLua(get_lua_state(), "API:CC_SPAWN()", "<CC:Event:Spawn a lv.90 pokemon>");
        }},
        {strings.event.reset_money, [] {
            executeLua(get_lua_state(), "API:CC_RESET_MONEY()", "<CC:Set money to 0>");
        }},
        {strings.event.moves_random, [] {
            executeLua(get_lua_state(), "API:CC_RANDOMIZE_MOVES()", "<CC:Randomize all moves>");
        }},
        {strings.event.team_random, [] {
            executeLua(get_lua_state(), "API:CC_RANDOMIZE_TEAM()", "<CC:Randomize team>");
        }},
        {strings.event.metronome_only, [] {
            executeLua(get_lua_state(), "API:CC_METRONOME()", "<CC:Metronome only>");
        }},
    };

    return eventsList;
}

HkTrampoline<void, void*, btl::Exp::CalcResult*> ExpMultiplierHook = hk::hook::trampoline([](void *calcParam, btl::Exp::CalcResult *result) {
    ExpMultiplierHook.orig(calcParam, result);

    CC_CHECK(expGain1k) {
        result->exp = static_cast<u32>(result->exp * 1000);
    }

    CC_CHECK(expGainZero) {
        result->exp = 0;
    }
});

HkTrampoline<void, btl::Capture::JudgeResult*, btl::Capture::JudgeParam*> CaptureHook = hk::hook::trampoline([](btl::Capture::JudgeResult* result, btl::Capture::JudgeParam* param) {
    CC_CHECK(neverCapture) {
        result->yureCount = 0;
        result->isCaptured = false;
        result->isCritical = false;
        return;
    }

    CC_CHECK(alwaysCapture) {
        param->isMustSuccess = true;
    }

    CaptureHook.orig(result, param);
});

static void reset() {
    CodePatcher cp(&btl::TYPE_EFFECT_TBL);

    Logger::log("Resetting patched table\n");
    for (int i = 0; i < 18; i++) {
        for (int j = 0; j < 18; j++) {
            cp.write(TYPE_EFFECT_TBL[i][j]);
        }
    }
}

HkTrampoline<void, btl::EventLauncher*, void*, void*, char, char, bool> TypeEffectHook = hk::hook::trampoline([](btl::EventLauncher* param_1, void* param_2, void* param_3, char param_4, char param_5, bool param_6) {
    CC_CHECK_NOT(invertTypeEffects) {
        CC_DISABLE(invertTypeEffects);

        if (!flags.disableTypeEffectsActive && !flags.randomTypeEffectsActive) {
            reset();
        }
    }

    CC_CHECK_NOT(disableTypeEffects) {
        CC_DISABLE(disableTypeEffects);

        if (!flags.invertTypeEffectsActive && !flags.randomTypeEffectsActive) {
            reset();
        }
    }

    CC_CHECK_NOT(randomTypeEffects) {
        CC_DISABLE(randomTypeEffects);

        if (!flags.disableTypeEffectsActive && !flags.invertTypeEffectsActive) {
            reset();
        }
    }

    return TypeEffectHook.orig(param_1, param_2, param_3, param_4, param_5, param_6);
});

static_assert(floatToFx(0.5) == 0x800);
static_assert(floatToFx(1.5) == 0x1800);
HkTrampoline<fx32, btl::EventLauncher*, void*, uint16_t> STABHook = hk::hook::trampoline([](btl::EventLauncher* param_1, void* param_2, uint16_t param_3) {
    auto ret = STABHook.orig(param_1, param_2, param_3);

    CC_CHECK(invertSTAB) {
        if (ret == floatToFx(1.5)) {
            return floatToFx(0.5);
        } else if (ret == floatToFx(0.5)) {
            return floatToFx(1.5);
        }
    }

    return ret;
});

std::vector<std::string> getEventList() {
    std::vector<std::string> out;

    for (const auto& evt : allEvents() | std::views::keys) {
        out.emplace_back(evt);
    }

    return out;
}

void triggerEvent(const std::string& eventName) {
    auto lang = CC_lang_current();
    auto strings = getStrings(lang);

    for (auto [evt, cb] : allEvents()) {
        if (eventName == evt) {
            char buffer[256] = {};
            sprintf(buffer, R"(API:pushNotice("%s\n%s"))", strings.poll.event, evt);
            executeLua(get_lua_state(), buffer, "<CC:EventLoop:EventTrigger>");
            cb();
            return;
        }
    }
}

void cc_hooks() {
    ExpMultiplierHook.installAtPtr(&btl::Exp::CalcExp);
    CaptureHook.installAtPtr(&btl::Capture::JudgeCapture);
    TypeEffectHook.installAtPtr(pun<void*>(&btl::EventLauncher::Event_CalcTypeMatchRatio));
    STABHook.installAtPtr(pun<void*>(&btl::EventLauncher::Event_CheckDamageAffinity));

    // ExpMultiplierHook::InstallAtOffset(0x01781ce8);
    // CaptureHook::InstallAtOffset(0x0197e0f4);
    // TypeEffectHook::InstallAtOffset(0x01330100);
    // STABHook::InstallAtOffset(0x02d0c404);
}
