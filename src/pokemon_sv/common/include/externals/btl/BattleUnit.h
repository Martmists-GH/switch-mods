#pragma once
#include <externals/util.h>

#include "MainModule.h"

namespace btl {
    struct WAZA_CORE {
        short wazaNo;
        uint8_t pp;
        uint8_t ppMax;
        uint8_t ppCnt;
        bool usedFlag;
        bool usedFlagFix;
        uint8_t usedCount;
        uint8_t killCount;
        EXTERNAL_PAD(1);
    };

    static_assert(sizeof(WAZA_CORE) == 10);
    static_assert(alignof(WAZA_CORE) == 2);

    struct WAZA_SET {
        btl::WAZA_CORE truth;
        btl::WAZA_CORE surface;
        uint8_t unk;
        EXTERNAL_PAD(1);
    };

    static_assert(sizeof(WAZA_SET) == 0x16);
    static_assert(sizeof(WAZA_SET[4]) == 0x58);
    static_assert(offsetof(WAZA_SET, surface) == 10);

    struct BattleUnit : ExternalType<BattleUnit> {
        struct vtable {
            EXTERNAL_PAD(0x98);
            int (*GetID)(BattleUnit*);
            EXTERNAL_PAD(0x50);
            short(*GetTokusei)(BattleUnit*);
        };

        struct fields {
            EXTERNAL_PAD(0x48);
            int myID;
            EXTERNAL_PAD(4);
            short hpMax;
            short hp;
            EXTERNAL_PAD(0x3ca);
            btl::WAZA_SET waza[4];
            short tokusei;
            EXTERNAL_PAD(2);
            uint8_t wazaCount;
            EXTERNAL_PAD(0x283);
            btl::MainModule* mainModule;
        };

        uint8_t GetClientID() {
            return this->impl()->fields.myID / 6;
        }

        int GetID() {
            return this->impl()->vtable->GetID(this);
        }

        int GetTokusei() {
            return this->impl()->vtable->GetTokusei(this);
        }

        void Setup(void* setupParam);
        void HpMinus(short amount);
    };

    static_assert(offsetof(BattleUnit::vtable, GetTokusei) == 0xf0);
    static_assert(offsetof(BattleUnit::instance, fields.waza) == 0x426);
    static_assert(offsetof(BattleUnit::instance, fields.mainModule) == 0x708);
}
