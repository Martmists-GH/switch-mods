#pragma once
#include <cstdlib>
#include <format>
#include <ios>
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
