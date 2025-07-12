#pragma once
#include <externals/fx32.h>

namespace btl {
    struct Capture {
        struct JudgeResult {
            bool isCaptured;
            bool isCritical;
            uint8_t yureCount;
        };

        struct JudgeParam {
            void* mainModule;
            void* battleEnv;
            void* userPoke;
            void* targetPoke;
            short itemID;
            fx32 captureValueCoef;
            bool isMustSuccess;
            bool isCriticalEnable;
        };

        static void JudgeCapture(JudgeResult* result, void* param);
    };
}
