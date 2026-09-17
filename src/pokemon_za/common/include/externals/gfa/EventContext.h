#pragma once

#include "externals/gfl/string.h"

namespace gfa {
    struct EventContext {
        int unk0;
        int unk1;
        int unk2;
        char unk3[4];
        gfl::StringView* param[10];
        void* unk4;

        static EventContext make(gfl::StringView* arg0) {
            return {
                .param[0] = arg0,
            };
        }
    };
}
