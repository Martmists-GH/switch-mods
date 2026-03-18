#pragma once
#include "BattleUnit.h"
#include "externals/fx32.h"

namespace btl {
    struct EventLauncher {
        fx32 Event_CalcTypeMatchRatio(btl::BattleUnit::instance* attacker, uint16_t wazaType);
        int Event_CheckDamageAffinity(btl::BattleUnit::instance* attacker, btl::BattleUnit::instance* defender, uint8_t wazaType, uint8_t defenderType, bool onlyAttacker);
        bool Event_SkipAvoidCheck(btl::BattleUnit::instance* attacker, btl::BattleUnit::instance* defender, void* waza);
    };
}
