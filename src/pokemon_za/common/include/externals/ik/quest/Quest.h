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

    struct Progress {
        int m_id;
        int m_nextId;
    };

    struct Quest {
        QuestString* m_id;
        QuestString* m_work;
        QuestString* m_label;
        char unk[0x18];
        gfl::Map<int32_t, Progress, int32_t> m_progress;

        Progress* GetCurrentProgress();
    };
}
