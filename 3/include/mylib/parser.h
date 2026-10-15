#pragma once

#include <optional>
#include <string>
#include <istream>
#include <sstream>
#include <cctype>

#include "drawRequest.h"

namespace mylib {

class Parser {
public:
    std::optional<DrawRequest> parse(std::istream& in);
private:
    std::optional<std::string> nextNonEmptyLine(std::istream& in);

    std::optional<int> parseInt(const std::string& s);

    std::optional<Point> parsePoint(const std::string& s);

    std::optional<Color> parseColor(const std::string& s);
};
} // namespace mylib