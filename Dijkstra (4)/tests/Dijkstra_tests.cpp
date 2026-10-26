#include <gtest/gtest.h>
#include <sstream>

#include "DijkstraSolver.h"

using namespace my_dijkstra_lib;

TEST(DijkstraSolverTest, FindsShortestPath) {
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
    DijkstraSolver solver(g);
    auto path = solver.findShortestPath("Moscow", "Kiev");
    ASSERT_TRUE(path.has_value());
    EXPECT_EQ(path.value().weight, 25u);
    EXPECT_EQ(path.value().vertices.size(), 4u);
    EXPECT_EQ(path.value().vertices[0], "Moscow");
    EXPECT_EQ(path.value().vertices[1], "Krasnoyarsk");
    EXPECT_EQ(path.value().vertices[2], "Omsk");
    EXPECT_EQ(path.value().vertices[3], "Kiev");
}

TEST(DijkstraSolverTest, HandlesNonExistentVertex) {
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
    DijkstraSolver solver(g);
    auto path = solver.findShortestPath("Moscow", "NonExistentCity");
    EXPECT_FALSE(path.has_value());
}

TEST(DijkstraSolverTest, HandlesEmptyGraph) {
    Graph g;
    DijkstraSolver solver(g);
    auto path = solver.findShortestPath("A", "B");
    EXPECT_FALSE(path.has_value());
}

TEST(DijkstraSolverTest, HandlesGraphWithNoPath) {
    std::istringstream in(
        "A B 10\n"
        "B C 5\n");
    auto g = GraphParser{}.parse(in);
    DijkstraSolver solver(g);
    auto path = solver.findShortestPath("A", "C");
    EXPECT_FALSE(path.has_value());
}

TEST(DijkstraSolverTest, HandlesGraphWithOneVertex) {
    std::istringstream in("A\n");
    auto g = GraphParser{}.parse(in);
    DijkstraSolver solver(g);
    auto path = solver.findShortestPath("A", "A");
    ASSERT_TRUE(path.has_value());
    EXPECT_EQ(path.value().weight, 0u);
    EXPECT_EQ(path.value().vertices.size(), 1u);
    EXPECT_EQ(path.value().vertices[0], "A");
}

TEST(DijkstraSolverTest, HandlesGraphWithTwoVertices) {
    std::istringstream in("A B 5\n");
    auto g = GraphParser{}.parse(in);
    DijkstraSolver solver(g);
    auto path = solver.findShortestPath("A", "B");
    ASSERT_TRUE(path.has_value());
    EXPECT_EQ(path.value().weight, 5u);
    EXPECT_EQ(path.value().vertices.size(), 2u);
    EXPECT_EQ(path.value().vertices[0], "A");
    EXPECT_EQ(path.value().vertices[1], "B");
}

TEST(DijkstraSolverTest, HandlesGraphWithOneEdge) {
    std::istringstream in("A B 5\n");
    auto g = GraphParser{}.parse(in);
    DijkstraSolver solver(g);
    auto path = solver.findShortestPath("A", "B");
    ASSERT_TRUE(path.has_value());
    EXPECT_EQ(path.value().weight, 5u);
    EXPECT_EQ(path.value().vertices.size(), 2u);
    EXPECT_EQ(path.value().vertices[0], "A");
    EXPECT_EQ(path.value().vertices[1], "B");
}

TEST(DijkstraSolverTest, HandlesGraphWithMultipleEdges) {
    std::istringstream in(
        "A B 5\n"
        "A C 10\n"
        "B C 3\n"
        "B D 2\n"
        "C D 1\n"
        "D E 4\n");
    auto g = GraphParser{}.parse(in);
    DijkstraSolver solver(g);
    auto path = solver.findShortestPath("A", "E");
    ASSERT_TRUE(path.has_value());
    EXPECT_EQ(path.value().weight, 9u);
    EXPECT_EQ(path.value().vertices.size(), 4u);
    EXPECT_EQ(path.value().vertices[0], "A");
    EXPECT_EQ(path.value().vertices[1], "B");
    EXPECT_EQ(path.value().vertices[2], "D");
    EXPECT_EQ(path.value().vertices[3], "E");
}