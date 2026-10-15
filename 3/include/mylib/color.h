#pragma once

#include <cstdint>

namespace mylib {

struct Color {
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;

    Color() = default;
    Color(int b_, int g_, int r_);
};

inline bool operator==(const Color& a, const Color& b) {
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

inline bool operator!=(const Color& a, const Color& b) {
    return !(a == b);
}

} // namespace mylib