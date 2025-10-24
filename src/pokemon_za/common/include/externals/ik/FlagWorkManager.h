#pragma once

#include "externals/gfl/string.h"

namespace ik {
    struct FlagWorkManager {
        static FlagWorkManager s_instance;

        int32_t GetWorkValue(gfl::StringHolder* work);
        void SetWorkValue(gfl::StringHolder* work, uint32_t value);
    };
}
