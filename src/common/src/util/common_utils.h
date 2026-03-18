#pragma once
#include <checks.hpp>
#include <cstdlib>
#include <cxxabi.h>
#include <format>
#include <ios>

#include "MessageUtil.h"
#define HK_ASSERT_MSG(CONDITION, MSG)             \
do {                                              \
    const bool _condition_temp = (CONDITION);     \
    if (_condition_temp == false) {               \
        ::hk::diag::abortImpl(                    \
            ::hk::svc::BreakReason_Assert,        \
            ::hk::diag::ResultAssertionFailure(), \
            __FILE__,                             \
            __LINE__,                             \
            MSG);                                 \
    }                                             \
} while (0)

struct FreeDeleter {
    void operator()(void* p) const {
        std::free(p);
    }
};

static std::string dumpHex(void* ptr, size_t size) {
    auto bytes = static_cast<const uint8_t*>(ptr);
    std::string out;
    for (size_t i = 0; i < size; i++) {
        out.append(std::format("{:02x} ", bytes[i]));
    }
    return out;
}

inline void show_yuzu_warning() {
    if (get_host() == NXHost::YUZU) {
        MessageUtil::popup(0, "Detected Yuzu Emulator, press Details.", R"(This mod will not be disabled, but please note that no support will be offered.
If you are experiencing issues with Yuzu, do the following:

1. Reproduce the issue on Ryujinx or Switch
2. Did the same problem happen?
    Yes: Report the issue to me (using Ryujinx/Switch logs)
    No: Report the issue to Yuzu

If you are not using Yuzu but a derivative (Suyu, Citron, Eden, etc.) the same applies.)");
    }
}

static std::string demangle(const char* name) {
    int status = -1;
    std::unique_ptr<char, void(*)(void*)> res {
        __cxxabiv1::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };

    return (status==0) ? res.get() : name ;
}
