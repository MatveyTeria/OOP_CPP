#include "ReportData.h"
#include <stdexcept>
#include <fstream>

namespace {
std::string strip(const std::string& s){
    auto first = s.begin();
    auto last = s.end();
    while (first != last && std::isspace(*first)) ++first;
    while (last != first && std::isspace(*(last - 1))) ++last;
    return std::string(first, last);
}

Record parseLine(const std::string& line){
    const auto colon_pos = line.find(':');
    if (colon_pos == std::string::npos){
        throw std::runtime_error("Incorrect line format: " + line);
    }

    Record res;
    res.key = strip(line.substr(0, colon_pos));
    res.value = strip(line.substr(colon_pos + 1));
    
    return res;
}

}

void ReportData::loadFromFile(std::string filename){
    std::ifstream in(filename);
    if (!in) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    _records.clear();
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        _records.push_back(parseLine(line));
    }
}