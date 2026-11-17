#pragma once
#include <vector>
#include "Board.hpp"
#include "Robot.hpp"

// Read-only «срез» состояния на текущем ходу. Ссылки — не владеющие.
struct GameState {
    const Board& board;
    const std::vector<Robot>& robots;
    int self_index;   // индекс стратегии в robots
    int turn;         // 0-based
    int max_turns;
};
