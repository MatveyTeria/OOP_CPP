#pragma once
#include <ostream>
#include "IGameObserver.hpp"

class FastObserver final : public IGameObserver {
public:
    explicit FastObserver(std::ostream& out) : out_(out) {}

    void onGameStart(const Board&, const std::vector<Robot>&) override {
        out_ << "Game started\n";
    }
    void onTurnStart(int, const std::vector<Robot>&) override {}
    void onTurnEnd(int, const std::vector<Action>&,
                   const Board&, const std::vector<Robot>&) override {}
    void onGameEnd(const GameResult& result) override;

private:
    std::ostream& out_;
};