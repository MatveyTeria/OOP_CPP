#include "GreedyDistanceStrategy.hpp"

namespace {

// Один шаг по направлению к цели. Приоритет осей фиксирован (сначала row).
// На пустой сетке это всегда кратчайший путь.
Action stepToward(Position from, Position to) noexcept {
    if (from.row < to.row) return Action::Down;
    if (from.row > to.row) return Action::Up;
    if (from.col < to.col) return Action::Right;
    if (from.col > to.col) return Action::Left;
    return Action::Stay;
}

} // namespace

Action GreedyDistanceStrategy::chooseAction(const GameState& state) {
    const Board& board = state.board;
    const Position cur = state.robots[state.self_index].position();

    if (board.hasCoin(cur)) return Action::Stay;

    const auto coins = board.coinPositions();
    if (coins.empty()) return Action::Stay;

    // Линейный проход за минимумом по манхэттену.
    // std::min_element не подходит напрямую: нужен tie-break по (row, col),
    // а лямбда в min_element сравнивает только «a < b». Мы это выражаем
    // через явный проход — читается проще и без лишних лямбд.
    Position best = coins.front();
    int best_dist = cur.manhattanTo(best);
    for (std::size_t i = 1; i < coins.size(); ++i) {
        const Position& c = coins[i];
        const int d = cur.manhattanTo(c);
        const bool better_dist = d < best_dist;
        const bool tie_and_earlier =
            d == best_dist &&
            (c.row < best.row || (c.row == best.row && c.col < best.col));
        if (better_dist || tie_and_earlier) {
            best_dist = d;
            best = c;
        }
    }
    return stepToward(cur, best);
}