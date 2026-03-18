#pragma once
#include <externals/util.h>

namespace btl {
    struct Exp {
        struct CalcParam {
            EXTERNAL_PAD(0x18);
            bool didFight;
            bool isExpShareOn;
        };

        struct CalcResult {
            uint32_t exp;
            bool byGakusyuSouti;
            bool isContainBonus;
        };

        static void CalcExp(CalcParam *calcParam, CalcResult* pResult);
    };
}
