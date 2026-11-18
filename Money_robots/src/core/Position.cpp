#include "Position.hpp"

#include <cstdlib>
#include <algorithm>

int Position::manhattanTo(const Position& other) const noexcept {
    return std::abs(row - other.row) +
           std::abs(col - other.col);
}

bool Position::isInside(int rows, int cols) const noexcept {
    return row >= 0 && row < rows &&
           col >= 0 && col < cols;
}