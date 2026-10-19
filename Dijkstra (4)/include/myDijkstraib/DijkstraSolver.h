#pragma once

#include <string>
#include <vector>
#include <optional>

#include "path.h"
#include "graph.h"
#include "edge.h"

namespace my_dijkstra_lib
{

class DijkstraSolver {
public:
    DijkstraSolver(const Graph& g) : graph(g) {}
    std::optional<Path> findShortestPath(const std::string& from, const std::string& to) const;
private:
    const Graph& graph;
}

} // namespace my_dijkstra_lib

