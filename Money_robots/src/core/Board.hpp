#pragma once
#include <vector>
#include <cstdint>
#include "Position.hpp"

class Board {
public:
    // rows, cols > 0, иначе std::invalid_argument
    Board(int rows, int cols);

    [[nodiscard]] int rows() const noexcept { return rows_; }
    [[nodiscard]] int cols() const noexcept { return cols_; }

    // Бросает std::out_of_range при выходе за границы.
    [[nodiscard]] bool hasCoin(Position p) const;
    void placeCoin(Position p);          // идемпотентна
    [[nodiscard]] bool takeCoin(Position p); // true, если монета была

    [[nodiscard]] int remainingCoins() const noexcept;
    [[nodiscard]] std::vector<Position> coinPositions() const;

private:
    int rows_;
    int cols_;
    std::vector<std::uint8_t> coins_; // flat, row*cols+col
};