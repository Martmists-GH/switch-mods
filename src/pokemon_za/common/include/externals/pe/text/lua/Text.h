#pragma once

#include "externals/gfl/text/StrBuf.h"
#include "externals/gfl/string.h"
#include "externals/gfl/object.h"

namespace pe::text::lua {
    struct Text {
        static gfl::RefPointer<gfl::text::StrBuf> GetText(gfl::StringHolder* file, gfl::StringHolder* label);
    };
}
