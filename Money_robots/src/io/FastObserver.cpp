#include "FastObserver.hpp"

void FastObserver::onGameEnd(const GameResult& result) {
    out_ << "\n=== Results ===\n";
    for (int idx : result.ranking()) {
        const auto& p = result.players[idx];
        out_ << "  " << p.name
             << "  score=" << p.score
             << "  lastCoinTurn=" << p.last_coin_turn
             << "  distance=" << p.distance << '\n';
    }
}