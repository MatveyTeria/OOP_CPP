#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "edge.h"

namespace my_graphs_lib
{

class Graph {
private:
    std::unordered_map<std::string, std::vector<Edge>> list;
public:
    void addEdge(const std::string& from, const std::string& to, unsigned int weight) noexcept;

    const std::vector<Edge>& getAdjacentEdges(co0nst std::string& vertex) const {
        list.at(vertex);
    }
}

} // namespace my_graphs_lib

