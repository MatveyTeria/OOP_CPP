#pragma once

#include "IRobotStrategy.hpp"

class NearestCoinStrategy final : public IRobotStrategy {
public:
    [[nodiscard]] std::string name() const override {
        return "nearest";
    }

    [[nodiscard]] Action chooseAction(
        const GameState& state) override;
};