#pragma once
#include "ls/util.h"

namespace pml::pokepara {
    struct InitialSpec {
        EXTERNAL_PAD(0x28);
        int monsno;
        uint8_t formno;
        EXTERNAL_PAD(1);
        uint16_t level;
        uint16_t sex;
    };

    struct Factory {
        static void InitCoreData(void*, InitialSpec*);
    };
}
