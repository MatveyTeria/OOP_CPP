#pragma once
#include "IRobotStrategy.hpp"

class RandomStrategy final : public IRobotStrategy {
public:
    [[nodiscard]] std::string name() const override { return "random"; }
    [[nodiscard]] Action chooseAction(const GameState& state) override;
};