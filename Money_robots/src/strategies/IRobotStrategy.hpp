#pragma once
#include <string>
#include "Action.hpp"
#include "GameState.hpp"

class IRobotStrategy {
public:
    virtual ~IRobotStrategy() = default;
    [[nodiscard]] virtual std::string name() const = 0;
    [[nodiscard]] virtual Action chooseAction(const GameState& state) = 0;
};