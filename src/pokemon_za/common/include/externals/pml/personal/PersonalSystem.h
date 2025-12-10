#pragma once

namespace pml::personal {
    struct PersonalSystem {
        static bool CheckPokeExist(uint32_t monsno, uint32_t formno);
        static int GetMinExp(uint32_t monsno, uint32_t formno, uint32_t level);
    };
}
