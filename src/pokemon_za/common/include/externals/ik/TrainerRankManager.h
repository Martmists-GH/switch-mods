#pragma once

#include "externals/gfl/object.h"

namespace ik {
    struct TrainerRankManager : gfl::Singleton<TrainerRankManager> {
        char unk[0x8];
        int m_rank;
        int m_tickets;
        int m_ticketsInfinite;
    };
}
