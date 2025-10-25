#pragma once

namespace pml::pokepara {
    struct InitialSpec {
        char unk0[0x18];
        uint32_t id;
        int unk1;
        uint32_t colorRnd;
        char unk3[0x4];
        uint8_t rareTryCount;
        char unk4[0xcf];

        void FixInitSpec();
    };
}
