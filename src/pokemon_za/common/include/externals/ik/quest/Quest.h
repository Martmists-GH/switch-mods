#pragma once

#include "externals/gfl/string.h"
#include "externals/gfl/container.h"

namespace ik::quest {
    struct QuestString {  // TODO: Maybe a Flatbuffer String Type?
        char unk0[8];
        int unk1;
        int unk2;
        char pad;
        char m_start;
    };

    struct ConditionPart {
        QuestString* m_type;
        int m_op;
        QuestString* m_param[3];
        QuestString* m_text;
        int m_icon;
    };

    struct ConditionGroup {
        gfl::Vector<ConditionPart> m_conditions;
    };

    struct Condition {
        gfl::Vector<ConditionGroup> m_groups;
        EXTERNAL_PAD(8);
    };

    struct Purpose {
        QuestString* m_map;
        QuestString* m_object;
        Condition m_displayCondition;
    };

    struct PurposeGroup {
        QuestString* m_text;
        gfl::Vector<Purpose> m_items;
        EXTERNAL_PAD(8);
    };

    struct Progress {
        int m_id;
        int m_nextId;
        QuestString* m_summary;
        Condition m_condition;
        PurposeGroup m_purpose;
    };

    struct Reward {
        int m_type;
        QuestString* m_param[3];
    };

    struct Quest {
        QuestString* m_id;
        QuestString* m_work;
        QuestString* m_label;
        QuestString* m_client;
        int m_questNum;
        int m_subQuestNum;
        int m_priority;
        bool m_onHud;
        bool m_realQuest;
        EXTERNAL_PAD(2);
        gfl::Map<int32_t, Progress> m_progress;
        gfl::Vector<Reward> m_rewards;

        Progress* GetCurrentProgress();
    };
}
