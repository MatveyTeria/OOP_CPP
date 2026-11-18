// AppConfig.hpp
#pragma once
#include <cstdint>
#include <string>
#include <vector>

enum class RunMode { Detailed, Fast, Tournament };

struct AppConfig {
    bool help_requested = false;
    RunMode mode = RunMode::Fast;
    std::vector<std::string> strategies;
    int rows = 10;
    int cols = 10;
    int num_coins = 20;
    int max_turns = 50;
    int games = 1;
    std::uint32_t seed = 0;
};