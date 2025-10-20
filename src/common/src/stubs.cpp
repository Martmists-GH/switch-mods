#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <hk/diag/diag.h>

extern "C" {
    void* hk_malloc(size_t size);
    void hk_free(void* ptr);

    void* __libc_malloc(size_t size) {
        return hk_malloc(size);
    }

    void __libc_free(void* ptr) {
        hk_free(ptr);
    }
}

namespace std {
    void __libcpp_verbose_abort(const char* __format, ...) {
        va_list args;
        va_start(args, __format);
        ::hk::diag::abortImpl(
            ::hk::svc::BreakReason_User,
            ::hk::diag::ResultAbort(),
            __FILE__,
            __LINE__,
            __format,
            args
        );
        va_end(args);
    }
}
