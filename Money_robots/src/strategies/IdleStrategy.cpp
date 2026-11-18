#include "IdleStrategy.hpp"

Action IdleStrategy::chooseAction(const GameState&) {
    return Action::Stay;
}