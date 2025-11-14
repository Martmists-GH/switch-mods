#pragma once

namespace pml::pokepara {
    struct CalcTool {
        static uint32_t CorrectColorRndForRare(uint32_t id, uint32_t rnd);

        static uint32_t CorrectColorRndForNotRare(uint32_t param_1, uint32_t param_2) {
            return (((param_2 ^ param_1) & 0xFFF0) ^ ((param_2 ^ param_1) >> 16)) <= 0xF
                       ? (param_2 ^ 0x10)
                       :  param_2;
        }
    };
}
