#pragma once

#include "color.h"
#include "point.h"

namespace mylib
{
struct DrawRequest {
    std::string inputPath;
    Point start, end;
    Color color;
    std::string outputPath;
};
} // namespace mylib
