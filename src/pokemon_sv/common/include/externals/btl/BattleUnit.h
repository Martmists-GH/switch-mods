#pragma once
#include <externals/util.h>

#include "MainModule.h"

namespace btl {
    struct BattleUnit : ExternalType<BattleUnit> {
        struct fields {
            EXTERNAL_PAD(0x48);
            int myID;
            EXTERNAL_PAD(4);
            short hpMax;
            short hp;
            EXTERNAL_PAD(0x6a8);
            btl::MainModule* mainModule;
        };

        uint8_t GetClientID() {
            return this->impl()->fields.myID / 6;
        }

        void HpMinus(short amount);
    };

    static_assert(offsetof(BattleUnit::instance, fields.mainModule) == 0x708);
}
