#pragma once

#include <externals/ik/BoxSaveUtil.h>

#include "externals/gfl/object.h"

// TODO: Move
namespace ik {
    struct UnitParty : ExternalType<UnitParty> {
        struct vtable : gfl::Object::vtable {
            void* unk1;
            void* unk2;
            void* unk3;
            void* unk4;
            gfl::RefPointer<ik::PokemonParam>(*GetMemberPtr)(UnitParty* ptr, int index);
            void* unk5[9];
        };

        gfl::RefPointer<ik::PokemonParam> GetMemberPtr(int idx) {
            return impl()->vtable->GetMemberPtr(this, idx);
        }
    };
}

namespace cmn {
    struct GameData {
        static gfl::RefPointer<ik::UnitParty> GetPlayerParty();
    };
}
