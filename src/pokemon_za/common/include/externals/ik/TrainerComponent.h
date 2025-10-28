#pragma once

namespace ik {
    struct TrainerComponent {
        char unk[0x1a8];
        float m_megaEnergy;
        float m_megaEnergyMax;

        void SetMegaEnergy(float value);
    };
}
