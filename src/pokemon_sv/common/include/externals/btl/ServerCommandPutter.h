#pragma once
#include "BattleUnit.h"
#include "externals/util.h"

namespace btl {
    struct BattleUnit;

    struct EventSystem {

    };

    struct EventFactor {
        struct EventHandlerArgs {
            EXTERNAL_PAD(0x38);
            EventSystem* eventSystem;
        };
    };

    namespace common {
        void AddAdditionalTokusei(btl::EventFactor::EventHandlerArgs* args, short tokusei);
    }

    struct ServerCommandExecutor {
        EXTERNAL_PAD(0x10);
        void* battleEnv;
        EventSystem* eventSystem;

        static void AddTokuseiHandler(btl::EventSystem*, btl::BattleUnit* unit, short tokusei);
    };

    struct ServerCommandQueue {
        void Put_Common(int, int, int);
    };

    struct ServerCommandPutter {
        EXTERNAL_PAD(0x18);
        ServerCommandQueue* queue;
        ServerCommandExecutor* executor;
    };
}
