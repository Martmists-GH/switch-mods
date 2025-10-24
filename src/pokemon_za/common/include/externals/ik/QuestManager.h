#pragma once

#include "externals/gfl/object.h"
#include "externals/ik/Quest.h"

namespace ik {
    struct QuestManager : gfl::Singleton<QuestManager> {
        ik::Quest* m_mainQuests;
        ik::Quest* m_subQuests;
        ik::Quest* m_labQuests;

        void QuestComplete(gfl::StringHolder* quest, bool unk);
        void SendQuestProgressUpdate(ik::quest::Quest* quest, int type);
    };
}
