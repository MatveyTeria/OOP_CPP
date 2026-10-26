#include <gtest/gtest.h>
#include <sstream>

#include "myDijkstralib/graph.h"

using namespace my_dijkstra_lib;

TEST(GraphTest, AddsVertex) {
    Graph g;
    g.addEdge("A", "B", 1);
    EXPECT_TRUE(g.hasVertex("A"));
    EXPECT_TRUE(g.hasVertex("B"));
}

TEST(GraphTest, AddsEdge) {
    Graph g;
    g.addEdge("A", "B", 1);
    const auto& adj = g.getAdjacentEdges("A");
    ASSERT_EQ(adj.size(), 1u);
    EXPECT_EQ(adj[0].to, "B");
    EXPECT_EQ(adj[0].weight, 1u);
}

TEST(GraphTest, HandlesNonExistentVertex) {
    Graph g;
    EXPECT_THROW(g.getAdjacentEdges("A"), std::out_of_range);
}

TEST(GraphTest, AddsMultipleEdges) {
    Graph g;
    g.addEdge("A", "B", 1);
    g.addEdge("A", "C", 2);
    g.addEdge("B", "C", 3);
    const auto& adjA = g.getAdjacentEdges("A");
    ASSERT_EQ(adjA.size(), 2u);
    EXPECT_EQ(adjA[0].to, "B");
    EXPECT_EQ(adjA[1].to, "C");
    const auto& adjB = g.getAdjacentEdges("B");
    ASSERT_EQ(adjB.size(), 2u);
    EXPECT_EQ(adjB[0].to, "A");
    EXPECT_EQ(adjB[1].to, "C");
}

TEST(GraphTest, AddsEdgeWithLargeWeight) {
    Graph g;
    g.addEdge("A", "B", 4294967295);
    const auto& adj = g.getAdjacentEdges("A");
    ASSERT_EQ(adj.size(), 1u);
    EXPECT_EQ(adj[0].to, "B");
    EXPECT_EQ(adj[0].weight, 4294967295u);
}

TEST(GraphTest, AddsEdgeWithZeroWeight) {
    Graph g;
    g.addEdge("A", "B", 0);
    const auto& adj = g.getAdjacentEdges("A");
    ASSERT_EQ(adj.size(), 1u);
    EXPECT_EQ(adj[0].to, "B");
    EXPECT_EQ(adj[0].weight, 0u);
}

TEST(GraphTest, HandlesCrLfLineEndings) {
    Graph g;
    g.addEdge("A", "B", 5);
    EXPECT_EQ(g.getAdjacentEdges("A")[0].to, "B");
}

TEST(GraphTest, AddsSelfLoop) {
    Graph g;
    g.addEdge("A", "A", 1);
    const auto& adj = g.getAdjacentEdges("A");
    ASSERT_EQ(adj.size(), 1u);
    EXPECT_EQ(adj[0].to, "A");
    EXPECT_EQ(adj[0].weight, 1u);
}

TEST(GraphTest, ChecksVertexExistence) {
    Graph g;
    g.addEdge("A", "B", 1);
    EXPECT_TRUE(g.vertexExists("A"));
    EXPECT_FALSE(g.vertexExists("C"));
}

TEST(GraphTest, ChecksEdgeExistence) {
    Graph g;
    g.addEdge("A", "B", 1);
    EXPECT_TRUE(g.hasVertex("A"));
    EXPECT_TRUE(g.hasVertex("B"));
    const auto& adjA = g.getAdjacentEdges("A");
    ASSERT_EQ(adjA.size(), 1u);
    EXPECT_EQ(adjA[0].to, "B");
    EXPECT_EQ(adjA[0].weight, 1u);
}