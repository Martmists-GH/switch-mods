#pragma once

namespace btl {
    struct Exp {
        struct CalcResult {
            uint32_t exp;
            bool byGakusyuSouti;
            bool isContainBonus;
        };

        static void CalcExp(void *calcParam, CalcResult* pResult);
    };
}
