#pragma once
#include <cstdint>

struct GameConfig {
    int rows       = 10;
    int cols       = 10;
    int num_coins  = 20;
    int max_turns  = 50;
    std::uint32_t seed = 0;   // 0 = использовать std::random_device

    // Бросает std::invalid_argument при некорректных значениях
    void validate() const;
};