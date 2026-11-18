#pragma once
#include "Action.hpp"
#include "Board.hpp"
#include "Robot.hpp"
#include "GameResult.hpp"

class IGameObserver {
public:
    virtual ~IGameObserver() = default;
    virtual void onGameStart(const Board&, const std::vector<Robot>&) = 0;
    virtual void onTurnStart(int turn, const std::vector<Robot>&) = 0;
    virtual void onTurnEnd(int turn,
                           const std::vector<Action>& actions,
                           const Board& board,
                           const std::vector<Robot>& robots) = 0;
    virtual void onGameEnd(const GameResult&) = 0;
};