#pragma once

#include <externals/ik/BasePokemonParam.h>

#include "externals/gfl/object.h"

// TODO: Move
namespace ik {
    struct UnitParty : ExternalType<UnitParty> {
        struct vtable : gfl::Object<>::vtable {
            void* unk1;
            void* unk2;
            void* unk3;
            void* unk4;
            gfl::IntrusivePtr<ik::BasePokemonParam>(*GetMemberPtr)(UnitParty* ptr, int index);
            void* unk5[9];
        };

        struct fields {};

        gfl::IntrusivePtr<ik::BasePokemonParam> GetMemberPtr(int idx) {
            return impl()->vtable->GetMemberPtr(this, idx);
        }
    };
}

namespace cmn {
    struct GameData {
        static gfl::IntrusivePtr<ik::UnitParty> GetPlayerParty();
    };
}
