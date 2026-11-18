#pragma once
#include "IRobotStrategy.hpp"

class SweepStrategy final : public IRobotStrategy {
public:
    [[nodiscard]] std::string name() const override { return "sweep"; }
    [[nodiscard]] Action chooseAction(const GameState& state) override;
};