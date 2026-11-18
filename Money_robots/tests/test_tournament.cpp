#include <gtest/gtest.h>
#include <stdexcept>
#include "Tournament.hpp"

TEST(TournamentTest, RequiresAtLeastTwoStrategies) {
    const auto registry = StrategyRegistry::createDefault();
    TournamentConfig config;
    config.strategy_names = {"idle"};
    config.base_game_config = GameConfig{3, 3, 0, 1, 1};
    EXPECT_THROW((Tournament{config, registry}), std::invalid_argument);
}

TEST(TournamentTest, RequiresPositiveRepetitions) {
    const auto registry = StrategyRegistry::createDefault();
    TournamentConfig config;
    config.strategy_names = {"idle", "random"};
    config.base_game_config = GameConfig{3, 3, 0, 1, 1};
    config.repetitions_per_pairing = 0;
    EXPECT_THROW((Tournament{config, registry}), std::invalid_argument);
}

TEST(TournamentTest, PlaysEveryUniquePairingRequestedNumberOfTimes) {
    const auto registry = StrategyRegistry::createDefault();
    TournamentConfig config;
    config.strategy_names = {"idle", "random", "nearest"};
    config.base_game_config = GameConfig{4, 4, 0, 1, 10};
    config.repetitions_per_pairing = 2;

    Tournament tournament(config, registry);
    const auto result = tournament.run();
    ASSERT_EQ(result.stats.size(), 3u);
    for (const auto& stats : result.stats) {
        EXPECT_EQ(stats.games_played, 4); // две пары * два повтора
        EXPECT_GE(stats.wins, 0);
        EXPECT_LE(stats.wins, stats.games_played);
        EXPECT_EQ(stats.total_score, 0);
    }
    EXPECT_EQ(result.stats[0].name, "idle"); // одинаковые нулевые результаты; tie-break по имени
}

TEST(TournamentTest, UnknownStrategyFailsWhenGameIsCreated) {
    const auto registry = StrategyRegistry::createDefault();
    TournamentConfig config;
    config.strategy_names = {"idle", "not-registered"};
    config.base_game_config = GameConfig{3, 3, 0, 1, 1};
    Tournament tournament(config, registry);
    EXPECT_THROW((void)tournament.run(), std::invalid_argument);
}
