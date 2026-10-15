#include "mylib/color.h"

namespace 
{
    std::uint8_t clamp(int value, int mn, int mx) {
        if (value > mx) return mx;
        if (value < mn) return mn;
        return static_cast<std::uint8_t>(value);
    }
} // namespace 


namespace mylib
{

Color::Color(int b_, int g_, int r_){
    b = clamp(b_, 0, 255);
    g = clamp(g_, 0, 255);
    r = clamp(r_, 0, 255);
}

} // namespace mylib