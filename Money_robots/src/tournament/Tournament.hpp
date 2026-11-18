#pragma once

#include <string>
#include <vector>

#include "AppConfig.hpp"
#include "Game.hpp"
#include "StrategyRegistry.hpp"

struct TournamentConfig {
    std::vector<std::string> strategy_names;
    GameConfig base_game_config;
    int repetitions_per_pairing = 1;
};

struct StrategyStats {
    std::string name;
    int games_played = 0;
    int wins = 0;
    int total_score = 0;
};

struct TournamentResult {
    std::vector<StrategyStats> stats;
};

class Tournament {
public:
    Tournament(TournamentConfig config,
                const StrategyRegistry& registry);

    [[nodiscard]] TournamentResult run();

private:
    TournamentConfig config_;
    const StrategyRegistry& registry_;
};