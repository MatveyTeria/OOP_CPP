#pragma once

namespace mylib {

struct Point {
    int x = 0;
    int y = 0;
};

inline bool operator==(const Point& a, const Point& b) {
    return a.x == b.x && a.y == b.y;
}

inline bool operator!=(const Point& a, const Point& b) {
    return !(a == b);
}

} // namespace mylib