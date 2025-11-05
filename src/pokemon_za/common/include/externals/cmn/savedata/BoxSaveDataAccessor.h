#pragma once

#include "externals/gfl/object.h"

namespace cmn::savedata {
    struct BoxSaveDataAccessor : ExternalType<BoxSaveDataAccessor> {
        struct vtable : gfl::Object::vtable {};
        struct fields : gfl::Object::fields {
            char unk[0x50a];
            ushort teamPos[6][6];
        };

        static gfl::RefPointer<BoxSaveDataAccessor> GetInstance();
    };
}
