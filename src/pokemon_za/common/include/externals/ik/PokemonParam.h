#pragma once

#include "externals/gfl/object.h"
#include "externals/pml/pokepara/PokemonParam.h"

namespace ik {
    // TODO: Move
    struct PokemonParam : ExternalType<PokemonParam> {
        struct vtable : gfl::Object::vtable {};
        struct fields : gfl::Object::fields {
            void* unk;
            void* unk2;
            pml::pokepara::PokemonParam* m_pp;
        };
    };
}
