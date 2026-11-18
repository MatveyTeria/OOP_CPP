#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "Game.hpp"
#include "IdleStrategy.hpp"
#include "IGameObserver.hpp"

namespace {
class StayStrategy final : public IRobotStrategy {
public:
    explicit StayStrategy(std::string strategy_name) : name_(std::move(strategy_name)) {}
    std::string name() const override { return name_; }
    Action chooseAction(const GameState&) override { return Action::Stay; }
private:
    std::string name_;
};

class CountingObserver final : public IGameObserver {
public:
    int starts = 0;
    int turn_starts = 0;
    int turn_ends = 0;
    int ends = 0;
    void onGameStart(const Board&, const std::vector<Robot>&) override { ++starts; }
    void onTurnStart(int, const std::vector<Robot>&) override { ++turn_starts; }
    void onTurnEnd(int, const std::vector<Action>&, const Board&,
                   const std::vector<Robot>&) override { ++turn_ends; }
    void onGameEnd(const GameResult&) override { ++ends; }
};

std::vector<std::unique_ptr<IRobotStrategy>> twoStayStrategies() {
    std::vector<std::unique_ptr<IRobotStrategy>> strategies;
    strategies.push_back(std::make_unique<StayStrategy>("one"));
    strategies.push_back(std::make_unique<StayStrategy>("two"));
    return strategies;
}
}

TEST(GameTest, RejectsEmptyStrategyList) {
    EXPECT_THROW((Game{GameConfig{2, 2, 0, 1, 1}, {}}), std::invalid_argument);
}

TEST(GameTest, RejectsInvalidConfiguration) {
    EXPECT_THROW((Game{GameConfig{0, 2, 0, 1, 1}, twoStayStrategies()}),
                 std::invalid_argument);
    EXPECT_THROW((Game{GameConfig{2, 2, 5, 1, 1}, twoStayStrategies()}),
                 std::invalid_argument);
}

TEST(GameTest, ReturnsOneResultPerStrategy) {
    Game game(GameConfig{4, 4, 0, 3, 123}, twoStayStrategies());
    const GameResult result = game.run();
    ASSERT_EQ(result.players.size(), 2u);
    EXPECT_EQ(result.players[0].name, "one");
    EXPECT_EQ(result.players[1].name, "two");
    EXPECT_EQ(result.players[0].score, 0);
    EXPECT_EQ(result.players[1].score, 0);
    EXPECT_EQ(result.players[0].distance, 0);
    EXPECT_EQ(result.players[1].distance, 0);
}

TEST(GameTest, FixedSeedProducesRepeatableResults) {
    Game first(GameConfig{6, 6, 8, 12, 77}, twoStayStrategies());
    Game second(GameConfig{6, 6, 8, 12, 77}, twoStayStrategies());
    const auto a = first.run();
    const auto b = second.run();
    ASSERT_EQ(a.players.size(), b.players.size());
    for (std::size_t i = 0; i < a.players.size(); ++i) {
        EXPECT_EQ(a.players[i].score, b.players[i].score);
        EXPECT_EQ(a.players[i].last_coin_turn, b.players[i].last_coin_turn);
        EXPECT_EQ(a.players[i].distance, b.players[i].distance);
    }
}

TEST(GameTest, NotifiesObserverAboutLifecycle) {
    auto observer = std::make_shared<CountingObserver>();
    Game game(GameConfig{3, 3, 0, 4, 1}, twoStayStrategies(), observer);
    (void)game.run();
    EXPECT_EQ(observer->starts, 1);
    EXPECT_EQ(observer->turn_starts, 4);
    EXPECT_EQ(observer->turn_ends, 4);
    EXPECT_EQ(observer->ends, 1);
}

TEST(GameTest, StopsImmediatelyWhenThereAreNoCoins) {
    auto observer = std::make_shared<CountingObserver>();
    Game game(GameConfig{3, 3, 0, 20, 1}, twoStayStrategies(), observer);
    (void)game.run();
    EXPECT_EQ(observer->turn_starts, 1);
    EXPECT_EQ(observer->turn_ends, 1);
}


#include "GameConfig.hpp"
#include "GameResult.hpp"

TEST(GameConfigTest, ValidatesDimensionsCoinsAndTurns) {
    EXPECT_NO_THROW((GameConfig{2, 3, 6, 1, 0}.validate()));
    EXPECT_THROW((GameConfig{0, 3, 0, 1, 0}.validate()), std::invalid_argument);
    EXPECT_THROW((GameConfig{2, 3, -1, 1, 0}.validate()), std::invalid_argument);
    EXPECT_THROW((GameConfig{2, 3, 7, 1, 0}.validate()), std::invalid_argument);
    EXPECT_THROW((GameConfig{2, 3, 0, 0, 0}.validate()), std::invalid_argument);
}

TEST(GameResultTest, RankingUsesScoreThenLastCoinTurnThenDistanceThenName) {
    GameResult result;
    result.players = {
        {"zeta", 2, 4, 3, 0},
        {"beta", 2, 2, 8, 1},
        {"alpha", 2, 2, 5, 2},
        {"winner", 3, -1, 100, 3}
    };
    EXPECT_EQ(result.ranking(), (std::vector<int>{3, 2, 1, 0}));
}

TEST(GameResultTest, NeverCollectedCoinRanksAfterCollectedCoinOnTie) {
    GameResult result;
    result.players = {
        {"idle", 1, -1, 0, 0},
        {"collector", 1, 9, 100, 1}
    };
    EXPECT_EQ(result.ranking(), (std::vector<int>{1, 0}));
}

TEST(GameResultTest, WinnersReturnsAllPlayersWithMaximumScore) {
    GameResult result;
    result.players = {
        {"a", 1, 2, 1, 0},
        {"b", 3, 5, 9, 1},
        {"c", 3, 1, 10, 2},
        {"d", 0, -1, 0, 3}
    };
    EXPECT_EQ(result.winners(), (std::vector<int>{1, 2}));
}

TEST(GameResultTest, EmptyResultHasNoRankingOrWinners) {
    GameResult result;
    EXPECT_TRUE(result.ranking().empty());
    EXPECT_TRUE(result.winners().empty());
}
