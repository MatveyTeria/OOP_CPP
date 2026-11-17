#pragma once
#include <memory>
#include <vector>
#include "GameConfig.hpp"
#include "GameResult.hpp"
#include "IGameObserver.hpp"
#include "IRobotStrategy.hpp"

class Game {
public:
    // Каждая стратегия соответствует роботу по индексу.
    // strategies непустой (проверяется validate).
    Game(GameConfig cfg,
         std::vector<std::unique_ptr<IRobotStrategy>> strategies,
         std::shared_ptr<IGameObserver> observer = nullptr);

    // Запускает партию до конца. Повторный вызов — UB; лучше один раз.
    [[nodiscard]] GameResult run();

private:
    void initializeBoard();
    void initializeRobots();
    void applyActions(const std::vector<Action>& actions, int turn);
    [[nodiscard]] GameResult buildResult() const;

    GameConfig cfg_;
    Board board_;
    std::vector<Robot> robots_;
    std::vector<std::unique_ptr<IRobotStrategy>> strategies_;
    std::shared_ptr<IGameObserver> observer_;
    std::mt19937 rng_;
};