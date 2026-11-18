#include "GameConfig.hpp"

#include <stdexcept>

void GameConfig::validate() const {
    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument(
            "Rows and cols must be positive");
    }

    if (num_coins < 0 || num_coins > rows * cols) {
        throw std::invalid_argument(
            "Invalid number of coins");
    }

    if (max_turns <= 0) {
        throw std::invalid_argument(
            "Max turns must be positive");
    }
}