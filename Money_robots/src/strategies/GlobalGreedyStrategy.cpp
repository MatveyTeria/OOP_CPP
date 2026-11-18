#include "GlobalGreedyStrategy.hpp"

#include <algorithm>
#include <vector>

namespace {

// Сколько других монет в «квадрате» радиуса r вокруг target.
// Расстояние Чебышева: max(|dr|, |dc|) <= r.
int clusterSizeAround(const std::vector<Position>& coins,
                      const Position& target, int r) noexcept {
    int count = 0;
    for (const auto& c : coins) {
        if (c == target) continue;
        const int dr = std::abs(c.row - target.row);
        const int dc = std::abs(c.col - target.col);
        if (std::max(dr, dc) <= r) ++count;
    }
    return count;
}

// Один жадный шаг к target на пустой сетке
Action firstStepToward(const Position& from, const Position& to) noexcept {
    if (from.row < to.row) return Action::Down;
    if (from.row > to.row) return Action::Up;
    if (from.col < to.col) return Action::Right;
    if (from.col > to.col) return Action::Left;
    return Action::Stay;
}

} // namespace

Action GlobalGreedyStrategy::chooseAction(const GameState& state) {
    const Board& board = state.board;
    const Position cur = state.robots[state.self_index].position();

    // Уже на монете — ждём, чтобы движок её собрал на «прибытии».
    if (board.hasCoin(cur)) return Action::Stay;

    const auto coins = board.coinPositions();
    if (coins.empty()) return Action::Stay;

    // Сортируем копию (coinPositions возвращает vector по значению — его и сортируем).
    // Ключ 1: манхэттен до нас.
    // Ключ 2: плотность кластера вокруг монеты (больше = лучше).
    // Ключ 3-4: row/col для детерминизма.
    auto sorted = coins;
    std::sort(sorted.begin(), sorted.end(),
              [&](const Position& a, const Position& b) {
        const int da = cur.manhattanTo(a);
        const int db = cur.manhattanTo(b);
        if (da != db) return da < db;
        const int ca = clusterSizeAround(coins, a, 2);
        const int cb = clusterSizeAround(coins, b, 2);
        if (ca != cb) return ca > cb;
        if (a.row != b.row) return a.row < b.row;
        return a.col < b.col;
    });

    return firstStepToward(cur, sorted.front());
}