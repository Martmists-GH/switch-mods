#pragma once

namespace pml::battle {
    struct Exp {
        struct CalcResult {
            int exp;
            bool unk0;
            char pad[3];
        };
        struct CalcParam {
            float exp;
            int levelCap;
            int unk1[4];
            bool didFight;
            bool hasLuckyEgg;
            bool unk2[4];
            char unk3[2];
        };

        static void CalcExp(pml::battle::Exp::CalcResult* result, pml::battle::Exp::CalcParam* param);
    };
}
