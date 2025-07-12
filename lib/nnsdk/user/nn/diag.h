#pragma once

namespace nn::diag::detail {
    void VAbortImpl(char const*, char const*, char const*, int, nn::Result const*, nn::os::UserExceptionInfo const*, char const*, std::va_list&);
}
