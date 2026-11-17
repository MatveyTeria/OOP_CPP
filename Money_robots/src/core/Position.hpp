#pragma once

struct Position {
    int row = 0;
    int col = 0;
    
    bool operator==(const Position&) const = default;

    [[nodiscard]] int manhattanTo(const Position& other) const noexcept;
    [[nodiscard]] bool isInside(int rows, int cols) const noexcept;
};