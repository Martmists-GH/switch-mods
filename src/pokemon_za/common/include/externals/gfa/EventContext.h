#pragma once

#include "externals/gfl/string.h"

namespace gfa {
    struct EventContext {
        int unk0;
        int unk1;
        int unk2;
        char unk3[4];
        gfl::StringHolder* param[10];
        void* unk4;

        static EventContext make(gfl::StringHolder* arg0) {
            return {
                .param[0] = arg0,
            };
        }
    };
}
