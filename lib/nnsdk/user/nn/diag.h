#pragma once

namespace nn::diag {
    struct MetaLogContext { };
}

namespace nn::diag::detail {
#ifdef __RTLD_PAST_13XX__
    void VAbortImpl(char const*, char const*, char const*, int, nn::Result const*, nn::os::UserExceptionInfo const*, char const*, std::va_list&);
#else
    void VAbortImpl(char const*, char const*, char const*, int, nn::Result const*, nn::os::UserExceptionInfo const*, nn::diag::MetaLogContext const&, char const*, std::va_list);
#endif
}
