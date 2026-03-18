#pragma once

namespace pml::pokepara {
    struct InitialSpec {
        char unk0[0x10];
        long personalRnd;
        long colorRnd;
        long id;
        short monsNo;
        short formNo;
        short itemNo;
        short level;
        short sex;
        short seikaku;
        short seikakuHosei;
        char unk1;
        char tryRareCount;
        char talentHp;
        char unk2;
        // TODO: Finish
    };
}
