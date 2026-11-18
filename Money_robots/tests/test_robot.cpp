#include <gtest/gtest.h>
#include "Robot.hpp"

TEST(RobotTest, InitializesState) {
    Robot robot("alpha", 3, {2, 4});
    EXPECT_EQ(robot.name(), "alpha");
    EXPECT_EQ(robot.index(), 3);
    EXPECT_EQ(robot.position(), (Position{2, 4}));
    EXPECT_EQ(robot.score(), 0);
    EXPECT_EQ(robot.distanceTraveled(), 0);
    EXPECT_EQ(robot.lastCoinTurn(), -1);
}

TEST(RobotTest, MoveToCountsManhattanDistance) {
    Robot robot("alpha", 0, {1, 1});
    robot.moveTo({1, 4});
    EXPECT_EQ(robot.distanceTraveled(), 3);
    robot.moveTo({3, 4});
    EXPECT_EQ(robot.distanceTraveled(), 5);
    EXPECT_EQ(robot.position(), (Position{3, 4}));
}

TEST(RobotTest, StayingDoesNotIncreaseDistance) {
    Robot robot("alpha", 0, {1, 1});
    robot.moveTo({1, 1});
    EXPECT_EQ(robot.distanceTraveled(), 0);
}

TEST(RobotTest, CollectCoinUpdatesScoreAndLastTurn) {
    Robot robot("alpha", 0, {0, 0});
    robot.collectCoin(2);
    EXPECT_EQ(robot.score(), 1);
    EXPECT_EQ(robot.lastCoinTurn(), 2);
    robot.collectCoin(7);
    EXPECT_EQ(robot.score(), 2);
    EXPECT_EQ(robot.lastCoinTurn(), 7);
}
