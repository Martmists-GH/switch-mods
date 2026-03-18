#pragma once

#include "externals/gfl/string.h"

namespace ik {
    struct Flag {
        char unk[0x100];
        gfl::Set<gfl::FnvHash64> m_table;
    };

    struct FlagWorkManager : gfl::Singleton<FlagWorkManager> {
        struct fields {
            ik::Flag* holders[7];
            gfl::Map<gfl::FnvHash64, bool> m_system_flag;
            gfl::Map<gfl::FnvHash64, bool> m_event_flag;
            gfl::Map<gfl::FnvHash64, bool> m_temp_flag;
            gfl::Map<gfl::FnvHash64, uint32_t> m_system_work;
            gfl::Map<gfl::FnvHash64, uint32_t> m_quest_work;
            gfl::Map<gfl::FnvHash64, uint32_t> m_momiji_work;
            gfl::Map<gfl::FnvHash64, uint32_t> m_temp_work;
        };

        static FlagWorkManager s_instance;

        int32_t GetWorkValue(gfl::StringHolder* work);
        void SetWorkValue(gfl::StringHolder* work, uint32_t value);
        bool GetFlag(char* flag);
        void SetFlag(char* flag, bool value);
    };
}
