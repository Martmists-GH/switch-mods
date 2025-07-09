#pragma once
#include <vapours/results/results_common.hpp>

namespace nn::ro {
    nn::Result LookupSymbol(u64* funcAddress, const char* symbolName);
}
