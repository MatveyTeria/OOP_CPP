#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "edge.h"

namespace my_dijkstra_lib
{

class Graph {
private:
    std::unordered_map<std::string, std::vector<Edge>> adj_list;
public:
    void addEdge(const std::string& from, const std::string& to, unsigned int weight) noexcept;

    const std::vector<Edge>& getAdjacentEdges(const std::string& vertex) const {
        adj_list.at(vertex);
    }
}

} // namespace my_dijkstra_lib

