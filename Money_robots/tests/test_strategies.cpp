#include <gtest/gtest.h>
#include <array>
#include <memory>
#include <vector>

#include "Board.hpp"
#include "Robot.hpp"
#include "GameState.hpp"
#include "IdleStrategy.hpp"
#include "RandomStrategy.hpp"
#include "NearestCoinStrategy.hpp"
#include "GreedyDistanceStrategy.hpp"
#include "GlobalGreedyStrategy.hpp"
#include "SweepStrategy.hpp"

namespace {
GameState makeState(const Board& board, const std::vector<Robot>& robots,
                    std::uint32_t rng = 0, int self = 0) {
    return GameState{board, robots, self, 0, 10, rng};
}
}

TEST(IdleStrategyTest, AlwaysStays) {
    Board board(3, 3);
    board.placeCoin({1, 1});
    std::vector<Robot> robots{Robot{"idle", 0, {1, 1}}};
    IdleStrategy strategy;
    EXPECT_EQ(strategy.chooseAction(makeState(board, robots)), Action::Stay);
}

TEST(RandomStrategyTest, MapsRngValueToFiveActions) {
    Board board(3, 3);
    std::vector<Robot> robots{Robot{"random", 0, {1, 1}}};
    RandomStrategy strategy;
    const std::array<Action, 5> expected{
        Action::Up, Action::Down, Action::Left, Action::Right, Action::Stay
    };
    for (std::uint32_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(strategy.chooseAction(makeState(board, robots, i)), expected[i]);
    }
}

TEST(NearestCoinStrategyTest, MovesTowardNearestCoin) {
    Board board(5, 5);
    board.placeCoin({0, 0});
    board.placeCoin({4, 4});
    std::vector<Robot> robots{Robot{"nearest", 0, {2, 2}}};
    NearestCoinStrategy strategy;
    EXPECT_EQ(strategy.chooseAction(makeState(board, robots)), Action::Up);
}

TEST(NearestCoinStrategyTest, UsesFirstRowMajorCoinOnDistanceTie) {
    Board board(5, 5);
    board.placeCoin({0, 2});
    board.placeCoin({4, 2});
    std::vector<Robot> robots{Robot{"nearest", 0, {2, 2}}};
    NearestCoinStrategy strategy;
    EXPECT_EQ(strategy.chooseAction(makeState(board, robots)), Action::Up);
}

TEST(NearestCoinStrategyTest, StaysIfOnCoinOrNoCoins) {
    Board board(3, 3);
    board.placeCoin({1, 1});
    std::vector<Robot> robots{Robot{"nearest", 0, {1, 1}}};
    NearestCoinStrategy strategy;
    EXPECT_EQ(strategy.chooseAction(makeState(board, robots)), Action::Stay);

    Board empty(3, 3);
    EXPECT_EQ(strategy.chooseAction(makeState(empty, robots)), Action::Stay);
}

TEST(GreedyDistanceStrategyTest, ChoosesDeterministicTieBreak) {
    Board board(5, 5);
    board.placeCoin({4, 2});
    board.placeCoin({2, 4});
    std::vector<Robot> robots{Robot{"distance", 0, {2, 2}}};
    GreedyDistanceStrategy strategy;
    EXPECT_EQ(strategy.chooseAction(makeState(board, robots)), Action::Right);
}

TEST(GreedyDistanceStrategyTest, StaysOnCoinOrWithoutCoins) {
    Board board(3, 3);
    board.placeCoin({1, 1});
    std::vector<Robot> robots{Robot{"distance", 0, {1, 1}}};
    GreedyDistanceStrategy strategy;
    EXPECT_EQ(strategy.chooseAction(makeState(board, robots)), Action::Stay);

    Board empty(3, 3);
    EXPECT_EQ(strategy.chooseAction(makeState(empty, robots)), Action::Stay);
}

TEST(GlobalGreedyStrategyTest, MovesTowardClosestCoin) {
    Board board(5, 5);
    board.placeCoin({0, 0});
    board.placeCoin({4, 4});
    std::vector<Robot> robots{Robot{"greedy", 0, {2, 2}}};
    GlobalGreedyStrategy strategy;
    EXPECT_EQ(strategy.chooseAction(makeState(board, robots)), Action::Up);
}

TEST(GlobalGreedyStrategyTest, PrefersDenserClusterWhenDistancesTie) {
    Board board(7, 7);
    board.placeCoin({1, 3}); // isolated, distance 3 from (4,3)
    board.placeCoin({5, 3}); // clustered, distance 1? use equal distance below
    board.placeCoin({5, 4});
    board.placeCoin({5, 2});
    std::vector<Robot> robots{Robot{"greedy", 0, {3, 3}}};
    GlobalGreedyStrategy strategy;
    // Both candidate targets are distance 2; lower target has more nearby coins.
    board.takeCoin({1, 3});
    board.placeCoin({1, 3});
    EXPECT_EQ(strategy.chooseAction(makeState(board, robots)), Action::Down);
}

TEST(GlobalGreedyStrategyTest, StaysOnCoinOrWithoutCoins) {
    Board board(3, 3);
    board.placeCoin({1, 1});
    std::vector<Robot> robots{Robot{"greedy", 0, {1, 1}}};
    GlobalGreedyStrategy strategy;
    EXPECT_EQ(strategy.chooseAction(makeState(board, robots)), Action::Stay);
    Board empty(3, 3);
    EXPECT_EQ(strategy.chooseAction(makeState(empty, robots)), Action::Stay);
}

TEST(SweepStrategyTest, FollowsSnakePattern) {
    Board board(3, 4);
    SweepStrategy strategy;
    std::vector<Robot> robots{Robot{"sweep", 0, {0, 0}}};
    EXPECT_EQ(strategy.chooseAction(makeState(board, robots)), Action::Right);
    robots[0].moveTo({0, 3});
    EXPECT_EQ(strategy.chooseAction(makeState(board, robots)), Action::Down);
    robots[0].moveTo({1, 3});
    EXPECT_EQ(strategy.chooseAction(makeState(board, robots)), Action::Left);
    robots[0].moveTo({1, 0});
    EXPECT_EQ(strategy.chooseAction(makeState(board, robots)), Action::Down);
}
