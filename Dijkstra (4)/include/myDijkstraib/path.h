#pragma once

#include <string>
#include <vector>

namespace my_dijkstra_lib
{

struct Path {
    std::vector<std::string> verticies;
    std::uint64_t totalWeight;
}

} // namespace my_dijkstra_lib

