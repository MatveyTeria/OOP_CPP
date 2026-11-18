#include <gtest/gtest.h>
#include <stdexcept>
#include <algorithm>
#include "Board.hpp"

TEST(BoardTest, RejectsNonPositiveDimensions) {
    EXPECT_THROW((Board{0, 3}), std::invalid_argument);
    EXPECT_THROW((Board{3, 0}), std::invalid_argument);
    EXPECT_THROW((Board{-1, 3}), std::invalid_argument);
}

TEST(BoardTest, StartsEmptyAndReportsDimensions) {
    Board board(2, 4);
    EXPECT_EQ(board.rows(), 2);
    EXPECT_EQ(board.cols(), 4);
    EXPECT_EQ(board.remainingCoins(), 0);
    EXPECT_TRUE(board.coinPositions().empty());
}

TEST(BoardTest, PlacesAndTakesCoin) {
    Board board(3, 3);
    board.placeCoin({1, 2});
    EXPECT_TRUE(board.hasCoin({1, 2}));
    EXPECT_EQ(board.remainingCoins(), 1);
    EXPECT_TRUE(board.takeCoin({1, 2}));
    EXPECT_FALSE(board.hasCoin({1, 2}));
    EXPECT_FALSE(board.takeCoin({1, 2}));
    EXPECT_EQ(board.remainingCoins(), 0);
}

TEST(BoardTest, PlacingCoinTwiceIsIdempotent) {
    Board board(2, 2);
    board.placeCoin({0, 1});
    board.placeCoin({0, 1});
    EXPECT_EQ(board.remainingCoins(), 1);
}

TEST(BoardTest, CoinPositionsAreInRowMajorOrder) {
    Board board(3, 4);
    board.placeCoin({2, 1});
    board.placeCoin({0, 3});
    board.placeCoin({1, 0});
    const std::vector<Position> expected{{0, 3}, {1, 0}, {2, 1}};
    EXPECT_EQ(board.coinPositions(), expected);
}

TEST(BoardTest, RejectsOutOfBoundsPositions) {
    Board board(2, 3);
    EXPECT_THROW(board.hasCoin({-1, 0}), std::out_of_range);
    EXPECT_THROW(board.placeCoin({0, 3}), std::out_of_range);
    EXPECT_THROW(board.takeCoin({2, 0}), std::out_of_range);
}
