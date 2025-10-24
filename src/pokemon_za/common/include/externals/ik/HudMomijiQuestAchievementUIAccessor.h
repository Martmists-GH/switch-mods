#pragma once

namespace ik {
    struct CompletionLevelData {
        int level;
        int points;
        int pointsNextLevel;
        float progress;
    };

    struct QuestCompletion {
        char unk[0x78];
        CompletionLevelData m_before;
        CompletionLevelData m_after;

        QuestCompletion(gfl::SizedHeap* heap, ik::quest::Quest* quest, int type);
        ~QuestCompletion();
    };


    struct HudMomijiQuestAchievementUIAccessor {
        static HudMomijiQuestAchievementUIAccessor* s_instance;  // TODO: This might be a field on some other type?

        void AddCompletion(QuestCompletion* completion);
    };
}
