#include "RandomStrategy.hpp"

Action RandomStrategy::chooseAction(const GameState& state) {
    // static constexpr — таблица собрана на этапе компиляции,
    // живёт в .rodata, никаких аллокаций при каждом вызове.
    static constexpr Action kActions[] = {
        Action::Up, Action::Down, Action::Left, Action::Right, Action::Stay
    };
    return kActions[state.rng_value % 5u];
}