#pragma once

#include <format>
#include "logger/logger.h"
#include "externals/gfl/text/StrBuf.h"
#include "externals/gfl/string.h"
#include "externals/gfl/object.h"

namespace pe::text::lua {
    struct Text {
        static void LoadMsgData(gfl::StringHolder* path);
        static void LoadMsgData(const char* path) {
            auto pathStr = gfl::StringHolder::Create(path);
            pe::text::lua::Text::LoadMsgData(&pathStr);
        }

        static gfl::RefPointer<gfl::text::StrBuf> GetText(gfl::StringHolder* file, gfl::StringHolder* label);
        inline static gfl::RefPointer<gfl::text::StrBuf> GetText(const char* file, const char* label) {
            auto fileHolder = gfl::StringHolder::Create(file);
            auto labelHolder = gfl::StringHolder::Create(label);
            return GetText(&fileHolder, &labelHolder);
        }
    };
}
