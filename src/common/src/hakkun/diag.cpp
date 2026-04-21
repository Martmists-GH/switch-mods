
#include <cstdarg>
#include <format>
#include <hk/types.h>
#include <hk/diag/diag.h>
#include "hk/svc/api.h"
#include "hk/svc/types.h"
#include <logger/logger.h>
#include <util/MessageUtil.h>

namespace hk::diag {
    hk_noreturn void abortImpl(svc::BreakReason reason, Result result, const char* file, int line, const char* msgFmt, std::va_list arg) {
        va_list argCopy;
        va_copy(argCopy, arg);
        auto len = std::vsnprintf(nullptr, 0, msgFmt, argCopy);
        va_end(argCopy);

        char buffer[std::max(len + 1, 19)];
        memset(buffer, 0, len + 1);
        if (len > 0) {
            std::vsnprintf(buffer, len, msgFmt, arg);
            svc::OutputDebugString(buffer, strlen(buffer));
            buffer[len] = '\0';
        } else {
            strcpy(buffer, "(formatting error)");
            buffer[18] = '\0';
        }

        if (strstr(buffer, "UnresolvedSymbol") != nullptr) {
            MessageUtil::abort("Wrong Version!", "Aborted at line %d in %s:\n%s", line, file, buffer);
        } else {
            MessageUtil::abort("!!!Hakkun Abort!!!", "Aborted at line %d in %s:\n%s", line, file, buffer);
        }
    }

    hk_noreturn void abortImpl(svc::BreakReason reason, Result result, const char* file, int line, const char* msgFmt, ...) {
        std::va_list arg;
        va_start(arg, msgFmt);
        abortImpl(reason, result, file, line, msgFmt, arg);
        va_end(arg);
    }

#if not defined(HK_RELEASE) or defined(HK_RELEASE_DEBINFO)
    void logLine(char const* msg, ...) {
        va_list arg;
        va_start(arg, msg);
        auto len = vsnprintf(nullptr, 0, msg, arg);
        char buf[len + 2];
        vsnprintf(buf, len + 2, msg, arg);
        buf[len] = '\n';
        hk::svc::OutputDebugString(buf, len + 1);
        va_end(arg);
    }
#endif
}
