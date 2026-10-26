#include <gtest/gtest.h>
#include <sstream>

#include "myDijkstralib/graphParser.h"

using namespace my_dijkstra_lib;

TEST(GraphParserTest, ParsesSingleEdge) {
    std::istringstream in("Moscow Novosibirsk 7\n");
    auto g = GraphParser{}.parse(in);
    EXPECT_TRUE(g.hasVertex("Moscow"));
    EXPECT_TRUE(g.hasVertex("Novosibirsk"));
    const auto& adj = g.getAdjacentEdges("Moscow");
    ASSERT_EQ(adj.size(), 1u);
    EXPECT_EQ(adj[0].to, "Novosibirsk");
    EXPECT_EQ(adj[0].weight, 7u);
}

TEST(GraphParserTest, ParsesExampleFromTask) {
    std::istringstream in(
        "Moscow Novosibirsk 7\n"
        "Moscow Toronto 9\n"
        "Moscow Krasnoyarsk 14\n"
        "Novosibirsk Toronto 10\n"
        "Novosibirsk Omsk 15\n"
        "Omsk Toronto 11\n"
        "Toronto Krasnoyarsk 2\n"
        "Krasnoyarsk Kiev 9\n"
        "Kiev Omsk 6\n");
    auto g = GraphParser{}.parse(in);
    EXPECT_EQ(g.getAdjacentEdges("Toronto").size(), 4u);
}

TEST(GraphParserTest, UndirectedAddsBothDirections) {
    std::istringstream in("A B 5\n");
    auto g = GraphParser{}.parse(in);
    EXPECT_EQ(g.getAdjacentEdges("A").size(), 1u);
    EXPECT_EQ(g.getAdjacentEdges("B").size(), 1u);
}

TEST(GraphParserTest, HyphenSeparator) {
    std::istringstream in("Moscow-Novosibirsk-7\n");
    auto g = GraphParser{}.parse(in);
    EXPECT_EQ(g.getAdjacentEdges("Moscow")[0].to, "Novosibirsk");
}

TEST(GraphParserTest, CommaSeparator) {
    std::istringstream in("Moscow, Novosibirsk, 7\n");
    auto g = GraphParser{}.parse(in);
    EXPECT_EQ(g.getAdjacentEdges("Moscow")[0].weight, 7u);
}

TEST(GraphParserTest, TabSeparator) {
    std::istringstream in("Moscow\tNovosibirsk\t7\n");
    auto g = GraphParser{}.parse(in);
    EXPECT_EQ(g.getAdjacentEdges("Moscow")[0].to, "Novosibirsk");
}

TEST(GraphParserTest, MultipleConsecutiveSeparators) {
    std::istringstream in("Moscow---Novosibirsk---7\n");
    auto g = GraphParser{}.parse(in);
    EXPECT_EQ(g.getAdjacentEdges("Moscow")[0].to, "Novosibirsk");
}

TEST(GraphParserTest, SeparatorsAtLineEdges) {
    std::istringstream in("-Moscow-Novosibirsk-7-\n");
    auto g = GraphParser{}.parse(in);
    EXPECT_EQ(g.getAdjacentEdges("Moscow")[0].to, "Novosibirsk");
}

TEST(GraphParserTest, CustomSeparator) {
    std::istringstream in("Moscow;Novosibirsk;7\n");
    auto g = GraphParser{";"}.parse(in);
    EXPECT_EQ(g.getAdjacentEdges("Moscow")[0].to, "Novosibirsk");
}

TEST(GraphParserTest, SkipsEmptyLines) {
    std::istringstream in("\n\nA B 5\n\n\n");
    auto g = GraphParser{}.parse(in);
    EXPECT_EQ(g.getAdjacentEdges("A").size(), 1u);
}

TEST(GraphParserTest, SkipsWhitespaceOnlyLines) {
    std::istringstream in("   \n\t\nA B 5\n");
    auto g = GraphParser{}.parse(in);
    EXPECT_EQ(g.getAdjacentEdges("A").size(), 1u);
}

TEST(GraphParserTest, EmptyStreamProducesEmptyGraph) {
    std::istringstream in("");
    auto g = GraphParser{}.parse(in);
    EXPECT_FALSE(g.hasVertex("A"));
}

TEST(GraphParserTest, TooFewFieldsThrows) {
    std::istringstream in("A B\n");
    EXPECT_THROW(GraphParser{}.parse(in), ParseError);
}

TEST(GraphParserTest, TooManyFieldsThrows) {
    std::istringstream in("A B 5 extra\n");
    EXPECT_THROW(GraphParser{}.parse(in), ParseError);
}

TEST(GraphParserTest, NonNumericWeightThrows) {
    std::istringstream in("A B abc\n");
    EXPECT_THROW(GraphParser{}.parse(in), ParseError);
}

TEST(GraphParserTest, WeightOverflowThrows) {
    std::istringstream in("A B 99999999999999999999\n");
    EXPECT_THROW(GraphParser{}.parse(in), ParseError);
}

TEST(GraphParserTest, HandlesCrLfLineEndings) {
    std::istringstream in("A B 5\r\nC D 3\r\n");
    auto g = GraphParser{}.parse(in);
    ASSERT_TRUE(g.hasVertex("A"));
    ASSERT_TRUE(g.hasVertex("C"));
    EXPECT_EQ(g.getAdjacentEdges("A")[0].to, "B"); // не "B\r"
    EXPECT_EQ(g.getAdjacentEdges("C")[0].to, "D");
}

TEST(GraphParserTest, ZeroWeightIsValid) {
    std::istringstream in("A B 0\n");
    auto g = GraphParser{}.parse(in);
    EXPECT_EQ(g.getAdjacentEdges("A")[0].weight, 0u);
}

TEST(GraphParserTest, MaxWeightIsValid) {
    std::istringstream in("A B 4294967295\n"); // UINT_MAX
    auto g = GraphParser{}.parse(in);
    EXPECT_EQ(g.getAdjacentEdges("A")[0].weight, 4294967295u);
}

TEST(GraphParserTest, ErrorMessageContainsLineNumber) {
    std::istringstream in("A B 5\nX Y\n");
    try {
        GraphParser{}.parse(in);
        FAIL() << "expected ParseError";
    } catch (const ParseError& e) {
        EXPECT_NE(std::string(e.what()).find("line 2"), std::string::npos)
            << "actual message: " << e.what();
    }
}