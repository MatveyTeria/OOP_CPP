#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "graph.h"

namespace my_dijkstra_lib
{

class GraphParser {
public:
    Graph parseFromFile(std::istream& in, std::string separators = " \t-,");
private:
    std::optional<std::string> nextNonEmptyLine(std::istream& in);

    std::optional<int> parseInt(const std::string& s);

    std::optional<Color> parseString(const std::string& s);
}

} // namespace my_dijkstra_lib

