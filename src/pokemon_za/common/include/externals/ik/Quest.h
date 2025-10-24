#pragma once

#include "externals/gfl/container.h"
#include "externals/gfl/hash.h"
#include "externals/ik/quest/Quest.h"

namespace ik {
    struct Quest {
        char unk[0x118];
        gfl::Map<gfl::FnvHash64, ik::quest::Quest> m_items;
    };
}
