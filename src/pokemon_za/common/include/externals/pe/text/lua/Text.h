#pragma once

#include <format>
#include "logger/logger.h"
#include "externals/gfl/text/StrBuf.h"
#include "externals/gfl/string.h"
#include "externals/gfl/object.h"

namespace pe::text::lua {
    struct Text {
        static void LoadMsgData(gfl::StringView* path);
        static void LoadMsgData(const char* path) {
            auto pathStr = gfl::StringView::Create(path);
            pe::text::lua::Text::LoadMsgData(&pathStr);
        }

        static gfl::IntrusivePtr<gfl::text::StrBuf> GetText(gfl::StringView* file, gfl::StringView* label);
        inline static gfl::IntrusivePtr<gfl::text::StrBuf> GetText(const char* file, const char* label) {
            auto fileHolder = gfl::StringView::Create(file);
            auto labelHolder = gfl::StringView::Create(label);
            return GetText(&fileHolder, &labelHolder);
        }
    };
}
