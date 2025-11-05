#pragma once

#include "externals/gfl/object.h"
#include "externals/pml/pokepara/PokemonParam.h"

namespace ik {
    struct PokemonParam : ExternalType<PokemonParam> {
        struct vtable : gfl::Object::vtable {};
        struct fields : gfl::Object::fields {
            void* unk;
            void* unk2;
            pml::pokepara::PokemonParam* m_pp;
        };
    };

    struct BoxSaveUtil : gfl::Singleton<BoxSaveUtil> {
        static gfl::RefPointer<ik::PokemonParam> GetPokemonParam(int32_t tray, int32_t pos);
        static bool SetPokemonParam(int32_t tray, int32_t pos, gfl::RefPointer<ik::PokemonParam> obj);
    };
}
