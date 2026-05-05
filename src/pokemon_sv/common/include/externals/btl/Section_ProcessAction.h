#pragma once
#include "Section.h"

namespace btl {
    struct Section_ProcessAction : btl::Section {
        struct Result {};
        struct Description {
            void* action;
        };

        void Execute(Result* result, Description* desc);
    };
}
