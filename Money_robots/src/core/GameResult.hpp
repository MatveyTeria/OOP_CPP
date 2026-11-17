#pragma once
#include <string>
#include <vector>

struct RobotResult {
    std::string name;
    int score         = 0;
    int last_coin_turn = -1;  // -1 = монет не было
    int distance      = 0;
    int index         = 0;
};

struct GameResult {
    std::vector<RobotResult> players;
    // Отсортировано по: score↓, last_coin_turn↑, distance↑
    // (значение -1 в last_coin_turn трактуем как INT_MAX)
    [[nodiscard]] std::vector<int> ranking() const;
    [[nodiscard]] std::vector<int> winners() const; // все с максимальным score
};