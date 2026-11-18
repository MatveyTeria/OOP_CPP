#include <gtest/gtest.h>
#include "Position.hpp"

TEST(PositionTest, ManhattanDistance) {
    EXPECT_EQ((Position{1, 2}.manhattanTo({4, 6})), 7);
    EXPECT_EQ((Position{3, 3}.manhattanTo({3, 3})), 0);
    EXPECT_EQ((Position{0, 5}.manhattanTo({4, 1})), 8);
}

TEST(PositionTest, IsInsideUsesZeroBasedBounds) {
    EXPECT_TRUE((Position{0, 0}.isInside(3, 4)));
    EXPECT_TRUE((Position{2, 3}.isInside(3, 4)));
    EXPECT_FALSE((Position{-1, 0}.isInside(3, 4)));
    EXPECT_FALSE((Position{0, -1}.isInside(3, 4)));
    EXPECT_FALSE((Position{3, 0}.isInside(3, 4)));
    EXPECT_FALSE((Position{0, 4}.isInside(3, 4)));
}

TEST(PositionTest, EqualityComparesBothCoordinates) {
    EXPECT_EQ((Position{2, 5}), (Position{2, 5}));
    EXPECT_NE((Position{2, 5}), (Position{5, 2}));
}
