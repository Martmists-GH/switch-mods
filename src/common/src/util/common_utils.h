#pragma once
#include <cstdlib>
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
