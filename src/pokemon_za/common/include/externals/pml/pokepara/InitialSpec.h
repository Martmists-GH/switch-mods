#pragma once

namespace pml::pokepara {
    struct InitialSpec {
        char unk0[0x18];
        uint32_t id;
        int unk1;
        uint32_t colorRnd;
        char unk3[0xd4];

        void FixInitSpec();
    };
}
