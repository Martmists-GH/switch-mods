#pragma once
#include "externals/util.h"

namespace btl {
    struct BattleCounter {
        void* vtable;
        uint64_t turnCount;
        // ???
    };

    struct BattleEnv {
        EXTERNAL_PAD(0x2a8);
        btl::BattleCounter* m_counter;
    };

    struct BTL_POKEPARAM {
        EXTERNAL_PAD(0x68 + 0xc);
        short hp;
        EXTERNAL_PAD(7);
        char myID;

        char ClientID() {
            if (myID < 6) return 0;
            if (myID < 12) return 2;
            if (myID < 18) return 1;
            if (myID < 24) return 3;
            if (myID < 30) return 4;
            return 0;
        }

        void HpMinus(short amount);
    };

    struct EventLauncher {
        EXTERNAL_PAD(0x18);
        btl::BattleEnv* m_pBattleEnv;

        bool scEvent_SkipAvoidCheck(btl::BTL_POKEPARAM* attacker, btl::BTL_POKEPARAM* defender, void* waza);
    };

    struct Capture {
        struct JudgeParam {

        };

        struct JudgeResult {
            bool isCaptured;
            bool isCritical;
            uint8_t yureCount;
        };

        static void JudgeCapture(JudgeResult*, JudgeParam*);
    };

    struct Section_DecrementPP {
        struct Result {
            bool isDecrement;
        };

        struct Description {
            btl::BTL_POKEPARAM* poke;
            uint8_t wazaIndex;
            uint8_t volume;
        };

        void Execute(Result*, Description*);
    };
}
