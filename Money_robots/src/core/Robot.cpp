#include "Robot.hpp"

#include <utility>

Robot::Robot(std::string name, int index, Position start)
    : name_(std::move(name)), index_(index), pos_(start)
{}

void Robot::moveTo(Position new_pos) noexcept {
    // distanceTraveled — суммарное манхэттенское перемещение.
    // Если робот «пытался выйти за край», Game оставил его на месте
    // (см. applyActions), и сюда придёт pos == new_pos → +0.
    // Если реально сдвинулся — +1. Логика «+ что» живёт здесь,
    // а не в Game: это свойство робота, не движка.
    distance_ += pos_.manhattanTo(new_pos);
    pos_ = new_pos;
}

void Robot::collectCoin(int turn) noexcept {
    ++score_;
    // Игровое правило из задания: при равенстве очков выигрывает тот,
    // кто забрал последнюю монету РАНЬШЕ. Храним ход последнего взятия;
    // при сравнении в GameResult::ranking() -1 трактуем как «никогда».
    last_coin_turn_ = turn;
}