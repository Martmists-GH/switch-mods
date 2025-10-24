#pragma once

#include "externals/gfl/object.h"

namespace ik {
    struct ResearchLevelManager : gfl::Singleton<ResearchLevelManager> {
        uint32_t GetLevel();
        uint32_t GetPoint();
        uint32_t GetNextLevelPoint();
        float GetNextLevelProgress();
        bool CanLevelUp();
        void ExecuteLevelUp();
    };
}
