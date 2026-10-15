#include "Color.h"

struct DrawRequest {
    std::string inputPath;
    int x0, y0, x1, y1;
    Color color;
    std::string outputPath;
};