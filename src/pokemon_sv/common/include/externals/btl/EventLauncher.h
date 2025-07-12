#pragma once
#include "externals/fx32.h"

namespace btl {
    struct EventLauncher {
        fx32 Event_CalcTypeMatchRatio(void* attacker, uint16_t wazaType);
        int Event_CheckDamageAffinity(void* attacker, void* defender, uint8_t wazaType, uint8_t defenderType, bool onlyAttacker);
    };
}
