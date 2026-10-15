#include <gtest/gtest.h>
#include "mylib/color.h"

TEST(ColorTest, DefaultConstructorIsBlack) {
    mylib::Color c;
    EXPECT_EQ(c.r, 0);
    EXPECT_EQ(c.g, 0);
    EXPECT_EQ(c.b, 0);
}
TEST(ColorTest, ValidValuesPassThrough) {
    mylib::Color c(2, 2, 2);
    EXPECT_EQ(c.b, 2);
    EXPECT_EQ(c.g, 2);
    EXPECT_EQ(c.r, 2);
}
TEST(ColorTest, PrimaryColorsPreserveChannels) {
    mylib::Color c(10, 20, 30);
    EXPECT_EQ(c.b, 10);
    EXPECT_EQ(c.g, 20);
    EXPECT_EQ(c.r, 30);
}
TEST(ColorTest, NegativeValuesClampToZero) {
    mylib::Color c(-10, -20, -30);
    EXPECT_EQ(c.b, 0);
    EXPECT_EQ(c.g, 0);
    EXPECT_EQ(c.r, 0);
}
TEST(ColorTest, ValuesAbove255ClampTo255) {
    mylib::Color c(255, 256, 1000);
    EXPECT_EQ(c.b, 255);
    EXPECT_EQ(c.g, 255);
    EXPECT_EQ(c.r, 255);
}
TEST(ColorTest, MixedOutOfRangeClampsPerChannel) {
    mylib::Color c(10, 20, 30);
    EXPECT_EQ(c.b, 10);
    EXPECT_EQ(c.g, 20);
    EXPECT_EQ(c.r, 30);
}
TEST(ColorTest, EqualityComparesAllChannels) {
    mylib::Color c1(10, 20, 30);
    mylib::Color c2(10, 20, 33);
    EXPECT_NE(c1, c2);
    c2.r = 30;
    c2.g = 22;
    EXPECT_NE(c1, c2);
    c2.g = 20;
    c2.b = 11;
    EXPECT_NE(c1, c2);
    c2.b = 10;
    EXPECT_EQ(c1, c2);
}