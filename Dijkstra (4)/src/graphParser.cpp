#include <queue>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <limits>
#include <optional>
#include <utility>
#include <functional>
#include <algorithm>

#include "myDijkstralib/graphParser.h"

namespace my_dijkstra_lib
{

Graph GraphParser::parse(std::istream& in) const {
    Graph graph;
    std::size_t lineNo = 0;
    std::string line;
    while (std::getline(in, line)) {
        ++lineNo;
        if (line.find_first_not_of(" \t\r") == std::string::npos) continue; // Skippin empty lines
        auto fields = split(line);
        if (fields.size() != 3) {
            throw ParseError("line " + std::to_string(lineNo) +
                             ": expected 3 fields, got " + std::to_string(fields.size()));
        } // Invalid field number
        if (fields[0].empty() || fields[1].empty()) {
            throw ParseError("line " + std::to_string(lineNo) + ": empty vertex name");
        } // Empty strings
        unsigned int weight = parseWeight(fields[2], lineNo);
        graph.addEdge(fields[0], fields[1], weight);
        graph.addEdge(fields[1], fields[0], weight);
    }
    return graph;
}

unsigned int GraphParser::parseWeight(const std::string& s, std::size_t lineNo) const {
    if (s.empty() || s[0] == '-') {
        throw ParseError("line " + std::to_string(lineNo) + ": invalid weight '" + s + "'");
    }
    try {
        unsigned long value = std::stoul(s);
        if (value > std::numeric_limits<unsigned int>::max()) {
            throw ParseError("line " + std::to_string(lineNo) + ": weight out of range '" + s + "'");
        }
        return static_cast<unsigned int>(value);
    } catch (const std::invalid_argument&) {
        throw ParseError("line " + std::to_string(lineNo) + ": not a number '" + s + "'");
    } catch (const std::out_of_range&) {
        throw ParseError("line " + std::to_string(lineNo) + ": weight out of range '" + s + "'");
    }
}

std::vector<std::string> GraphParser::split(const std::string& line) const {
    std::vector<std::string> result;
    std::size_t pos = 0;
    while (pos < line.size()) {
        std::size_t start = line.find_first_not_of(separators_, pos);
        if (start == std::string::npos) break;
        std::size_t end = line.find_first_of(separators_, start);
        if (end == std::string::npos) end = line.size();
        result.push_back(line.substr(start, end - start));
        pos = end;
    }
    return result;
}

} // namespace my_dijkstra_lib

