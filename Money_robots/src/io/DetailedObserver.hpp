#pragma once
#include <iosfwd>
#include "ConsoleBoardRenderer.hpp"
#include "IGameObserver.hpp"

class DetailedObserver final : public IGameObserver {
public:
    // Потоки инжектируются — тестируем через stringstream.
    DetailedObserver(std::istream& in, std::ostream& out);

    void onGameStart(const Board& board,
                     const std::vector<Robot>& robots) override;
    void onTurnStart(int turn,
                     const std::vector<Robot>& robots) override;
    void onTurnEnd(int turn,
                   const std::vector<Action>& actions,
                   const Board& board,
                   const std::vector<Robot>& robots) override;
    void onGameEnd(const GameResult& result) override;

private:
    std::istream& in_;
    std::ostream& out_;
    ConsoleBoardRenderer renderer_;
};