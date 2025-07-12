#pragma once

using fx32 = int32_t;

inline constexpr float fxToFloat(const fx32 fx) {
    auto num = fx >> 12;
    auto dec = (fx & 0xfff) / 4096.0f;
    if (num < 0) {
        return num - dec;
    } else {
        return num + dec;
    }
}

inline constexpr fx32 floatToFx(const float f) {
    if (f > 0) {
        return static_cast<int>(f * (1 << 12) + 0.5f);
    } else {
        return static_cast<int>(f * (1 << 12) - 0.5f);
    }
}
