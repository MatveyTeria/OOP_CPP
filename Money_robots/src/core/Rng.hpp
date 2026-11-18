#pragma once
#include <string>
#include "Position.hpp"

class Robot {
public:
    Robot(std::string name, int index, Position start);

    [[nodiscard]] const std::string& name() const noexcept { return name_; }
    [[nodiscard]] int index() const noexcept { return index_; }
    [[nodiscard]] Position position() const noexcept { return pos_; }
    [[nodiscard]] int score() const noexcept { return score_; }
    [[nodiscard]] int distanceTraveled() const noexcept { return distance_; }
    // Ход (0-based), на котором робот взял последнюю монету, или -1
    [[nodiscard]] int lastCoinTurn() const noexcept { return last_coin_turn_; }

    // Мутаторы — вызываются только Game.
    void moveTo(Position new_pos) noexcept;   // +1 к distance, если реально сдвинулся
    void collectCoin(int turn) noexcept;      // score++, last_coin_turn_ = turn

private:
    std::string name_;
    int index_;
    Position pos_;
    int score_ = 0;
    int distance_ = 0;
    int last_coin_turn_ = -1;
};