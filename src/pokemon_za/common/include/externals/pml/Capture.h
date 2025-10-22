#pragma once

namespace pml {
    struct Capture {
        struct Param {
            void* userMon;
            void* targetMon;
            short ballItemID;
            char unk[30];
        };
        struct Result {
            bool isCaptured;
            bool isCritical;
            char unk1;
            uint8_t yureCount;
            int unk2[2];
            float m_indicator[12];
        };

        char unk[8];
        pml::Capture::Param m_param;
        pml::Capture::Result m_result;

        void Judge();
    };
}
