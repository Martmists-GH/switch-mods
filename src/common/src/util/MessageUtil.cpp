#include "MessageUtil.h"

#include <cstdio>
#include <hk/svc/api.h>
#include <nn/os.h>

#include "nn/err.h"

namespace MessageUtil {
    NORETURN void abort(u32 errcode, const std::string& message, const std::string& description) {
        abort(errcode, message.c_str(), description.c_str());
    }

    NORETURN void abort(u32 errcode, const char *message, const char *description, ...) {
        va_list args;
        va_start(args, description);
        abort(errcode, message, description, args);
        va_end(args);
    }

    NORETURN void abort(u32 errcode, const char *message, const char *description, va_list args) {
        popup(errcode, message, description, args);
        nn::os::SleepThread(nn::TimeSpan::FromSeconds(1));
        __asm(R"(
            svc 0x7
        )");
    }

    NORETURN void abort(const char *message, const char *description, ...) {
        va_list args;
        va_start(args, description);
        abort(0, message, description, args);
    }

    NORETURN void abort(const char *message, const char *description, va_list args) {
        abort(0, message, description, args);
    }

    NORETURN void abort(const char *message, ...) {
        va_list args;
        va_start(args, message);
        abort(0, "ABORT!", message, args);
    }

    NORETURN void abort(const char *message, va_list args) {
        abort(0, "ABORT!", message, args);
    }

    void popup(u32 errcode, const std::string& message, const std::string& description) {
        popup(errcode, message.c_str(), description.c_str());
    }

    void popup(u32 errcode, const char *message, const char *description, ...) {
        va_list args;
        va_start(args, description);
        popup(errcode, message, description, args);
        va_end(args);
    }

    void popup(u32 errcode, const char *message, const char *description, va_list args) {
        char buffer[0x2000] = {};
        if (vsnprintf(buffer, sizeof(buffer), description, args) >= 0) {
            nn::err::ApplicationErrorArg arg;
            arg.SetApplicationErrorCodeNumber(errcode);
            arg.SetDialogMessage(message);
            arg.SetFullScreenMessage(buffer);
            nn::err::ShowApplicationError(arg);
        }
    }
}
