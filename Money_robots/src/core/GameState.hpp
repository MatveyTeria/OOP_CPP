// GameState.hpp
#pragma once
#include <cstdint>
#include <vector>
#include "Board.hpp"
#include "Robot.hpp"

struct GameState {
    const Board& board;
    const std::vector<Robot>& robots;
    int self_index;
    int turn;
    int max_turns;
    std::uint32_t rng_value;
};