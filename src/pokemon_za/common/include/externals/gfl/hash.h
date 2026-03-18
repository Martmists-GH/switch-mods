#pragma once

namespace gfl {
    struct FnvHash64 {
        long value;

        explicit FnvHash64(long value) : value(value) {}
        explicit FnvHash64(const char* content) : value(gfl::FnvHash64::hash(content)) {}

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

        static constexpr long operator()(const char* value) {
            return hash(value);
        }

    private:
        static const long FNV_BASIS = 0xCBF29CE484222645;
        static const long FNV_PRIME = 0x00000100000001B3;
    };
}
