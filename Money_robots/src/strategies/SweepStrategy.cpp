#include "SweepStrategy.hpp"

Action SweepStrategy::chooseAction(const GameState& state) {
    const Board& board = state.board;
    const Position pos = state.robots[state.self_index].position();

    // Стоим на монете — ждём, чтобы движок её собрал на «прибытии».
    if (board.hasCoin(pos)) return Action::Stay;

    const int cols = board.cols();
    const int rows = board.rows();
    const bool going_right = (pos.row % 2 == 0);

    // Змейка: чётные строки вправо, нечётные влево, на краю — вниз.
    if (going_right  && pos.col < cols - 1) return Action::Right;
    if (!going_right && pos.col > 0)        return Action::Left;
    if (pos.row < rows - 1)                 return Action::Down;
    return Action::Stay;   // нижний правый угол — дальше некуда
}