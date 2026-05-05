#pragma once
#include "MainModule.h"

namespace btl {
    struct Section {
        void* vtable;
        btl::MainModule* m_mainModule;
        btl::BattleEnv* m_battleEnv;
    };
}
