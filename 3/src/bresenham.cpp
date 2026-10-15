#include "mylib/bresenham.h"

#define MAX_ITERATIONS 1000

namespace mylib
{

void drawLine(Image& img, Point a, Point b, Color c) {
    int dx = std::abs(b.x - a.x);
    int dy = std::abs(b.y - a.y);
    int sx = (a.x < b.x) ? 1 : -1;   // шаг по X
    int sy = (a.y < b.y) ? 1 : -1;   // шаг по Y

    bool steep = dy > dx;
    if (steep) {
        std::swap(dx, dy);
        std::swap(sx, sy);
    }
    Point start = steep? Point{a.y, a.x}: a;
    Point end = steep? Point{b.y, b.x}: b;

    int err = dx / 2;
    int x = start.x, y = start.y;

    int iter_count = 0;

    while (true) {
        if (steep)
            img.setPixel({y, x}, c);
        else
            img.setPixel({x, y}, c);

        if (x == end.x && y == end.y) break; // Reached the end
        if (iter_count > dx) break;

        err -= dy;
        if (err < 0) {
            y += sy;
            err += dx;
        }
        x += sx;
        iter_count++;
    }
}

} // namespace mylib
