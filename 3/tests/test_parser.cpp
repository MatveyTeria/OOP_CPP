#include <gtest/gtest.h>
#include "mylib/parser.h"

TEST(ParserTest, ParsesCompleteValidInput) {
    std::stringstream in(
        "kitty.bmp\n"
        "100 120\n"
        "380 160\n"
        "255 10 10\n"
        "kitty_line.bmp\n");

    mylib::Parser parser;
    auto req = parser.parse(in);
    ASSERT_TRUE(req.has_value());
    EXPECT_EQ(req->inputPath, "kitty.bmp");
    EXPECT_EQ(req->start.x, 100);
    EXPECT_EQ(req->start.y, 120);
    EXPECT_EQ(req->end.x, 380);
    EXPECT_EQ(req->end.y, 160);
    EXPECT_EQ(req->outputPath, "kitty_line.bmp");
    EXPECT_EQ(req->color.g, 10);
}

TEST(ParserTest, SkipsEmptyLines) {
    std::stringstream in(
        "\n"
        "    \n"
        "kitty.bmp\n"
        "\n"
        "100 120\n"
        "\n"
        "380 160\t\n"
        "   \n"
        "255 10 10\n"
        "   \t\n"
        "kitty_line.bmp\n");

    mylib::Parser parser;
    auto req = parser.parse(in);
    ASSERT_TRUE(req.has_value());
    EXPECT_EQ(req->inputPath, "kitty.bmp");
    EXPECT_EQ(req->start.x, 100);
    EXPECT_EQ(req->start.y, 120);
    EXPECT_EQ(req->end.x, 380);
    EXPECT_EQ(req->end.y, 160);
    EXPECT_EQ(req->outputPath, "kitty_line.bmp");
    EXPECT_EQ(req->color.g, 10);
}

TEST(ParserTest, IgnoreSpaces) {
    std::stringstream in(
        "kitty.bmp\n"
        "   100\t120\n"
        " 380   160\t\n"
        "  255    10  10\n"
        "kitty_line.bmp\n");

    mylib::Parser parser;
    auto req = parser.parse(in);
    ASSERT_TRUE(req.has_value());
    EXPECT_EQ(req->inputPath, "kitty.bmp");
    EXPECT_EQ(req->start.x, 100);
    EXPECT_EQ(req->start.y, 120);
    EXPECT_EQ(req->end.x, 380);
    EXPECT_EQ(req->end.y, 160);
    EXPECT_EQ(req->outputPath, "kitty_line.bmp");
    EXPECT_EQ(req->color.g, 10);
}

TEST(ParserNextLine, ReturnsNulloptOnEmptyStream) {
    std::stringstream in;

    mylib::Parser parser;
    auto req = parser.parse(in);
    ASSERT_FALSE(req.has_value());
}

