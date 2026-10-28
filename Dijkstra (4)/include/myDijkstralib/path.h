#pragma once

#include <string>
#include <vector>
#include <cstdint>


namespace my_dijkstra_lib
{

struct Path {
    std::vector<std::string> vertices;
    std::uint64_t totalWeight;
};

} // namespace my_dijkstra_lib

