#pragma once
#include "Section.h"

namespace btl {
    struct Section_DecrementPP : btl::Section {
        struct Result {
            bool isDecrement;
        };

        struct Description {
            btl::BattleUnit::instance* poke;
            uint8_t wazaIndex;
            uint8_t volume;
        };

        void Execute(Result*, Description*);
    };
}
