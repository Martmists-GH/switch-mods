#pragma once

namespace gfl {
    struct FnvHash64 {
        static long GetEmptyString() {
            return FNV_BASIS;
        }

        static constexpr long hash(const char* input) {
            long result = FNV_BASIS;

            char c;
            while ((c = *(input++), c != '\0')) {
                result = (result ^ c) * FNV_PRIME;
            }

            return result;
        }

    private:
        static const long FNV_BASIS = 0xCBF29CE484222645;
        static const long FNV_PRIME = 0x00000100000001B3;
    };
}
