#pragma once
#include "IRobotStrategy.hpp"

class IdleStrategy final : public IRobotStrategy {
public:
    [[nodiscard]] std::string name() const override { return "idle"; }
    [[nodiscard]] Action chooseAction(const GameState&) override;
};