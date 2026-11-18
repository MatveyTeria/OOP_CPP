#pragma once
#include "IRobotStrategy.hpp"

class GreedyDistanceStrategy final : public IRobotStrategy {
public:
    [[nodiscard]] std::string name() const override { return "distance"; }
    [[nodiscard]] Action chooseAction(const GameState& state) override;
};