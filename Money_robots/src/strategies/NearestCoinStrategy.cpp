#include "NearestCoinStrategy.hpp"

Action NearestCoinStrategy::chooseAction(
    const GameState& state) {

    const Board& board = state.board;
    const Position cur =
        state.robots[state.self_index].position();

    if (board.hasCoin(cur)) {
        return Action::Stay;
    }

    const auto coins = board.coinPositions();

    if (coins.empty()) {
        return Action::Stay;
    }

    Position best = coins.front();
    int best_dist = cur.manhattanTo(best);

    for (const auto& coin : coins) {
        const int dist = cur.manhattanTo(coin);

        if (dist < best_dist) {
            best = coin;
            best_dist = dist;
        }
    }

    if (cur.row < best.row) return Action::Down;
    if (cur.row > best.row) return Action::Up;
    if (cur.col < best.col) return Action::Right;
    if (cur.col > best.col) return Action::Left;

    return Action::Stay;
}