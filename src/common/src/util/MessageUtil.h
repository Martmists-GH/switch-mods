#pragma once

#include <stdarg.h>
#include <nn/primitives.h>
#include <string>

namespace MessageUtil {
    NORETURN void abort(u32 errcode, const std::string& message, const std::string& description);
    NORETURN void abort(u32 errcode, const char *message, const char *description, ...);
    NORETURN void abort(u32 errcode, const char *message, const char *description, va_list args);

    void popup(u32 errcode, const std::string& message, const std::string& description);
    void popup(u32 errcode, const char *message, const char *description, ...);
    void popup(u32 errcode, const char *message, const char *description, va_list args);
}
