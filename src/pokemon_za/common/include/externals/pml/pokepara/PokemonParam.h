#pragma once

#include "externals/pml/pokepara/CoreParam.h"

namespace pml::pokepara {
    struct PokemonParam : ExternalType<PokemonParam> {
        struct vtable : pml::pokepara::CoreParam::vtable {};
        struct fields : pml::pokepara::CoreParam::fields {};
    };
}
