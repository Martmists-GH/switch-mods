#pragma once

namespace pml::pokepara {
    struct InitialSpec {
        char unk0[0x18];
        uint32_t id;
        int unk1;
        uint32_t colorRnd;
        char unk3[0x4];
        uint8_t rareTryCount;
        char unk4;
        short monsNo;
        short formNo;
        short itemNo;
        char unk5[0x8];
        short level;
        short sex;
        short nature;
        short unk6;
        char abilityIndex;
        char unk7[0x7];
        short iv[6];
        short ev[6];
        char unk8[0x98];

        void FixInitSpec();
    };
}
