#pragma once
#include "IRobotStrategy.hpp"

class GlobalGreedyStrategy final : public IRobotStrategy {
public:
    [[nodiscard]] std::string name() const override { return "greedy"; }
    [[nodiscard]] Action chooseAction(const GameState& state) override;
};