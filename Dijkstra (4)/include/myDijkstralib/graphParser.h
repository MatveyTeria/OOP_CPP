#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "graph.h"

namespace my_dijkstra_lib
{

class ParseError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class GraphParser {
public:
    GraphParser(std::string separators = " \t-,") : separators_(separators) {}
    Graph parse(std::istream& in) const;
private:
    unsigned int parseWeight(const std::string& s, std::size_t lineNo) const;

     std::vector<std::string> split(const std::string& line) const;

    std::string separators_;
};

} // namespace my_dijkstra_lib

