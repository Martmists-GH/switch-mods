#pragma once

#include "externals/gfa/EventContext.h"

namespace ik {
    struct EventSystemCallFunctions {
        static void FixWeather(gfa::EventContext* context);
    };
}
