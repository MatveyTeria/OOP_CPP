#include "mylib/parser.h"
#include "mylib/point.h"
#include "mylib/color.h"


namespace mylib {

std::optional<DrawRequest> Parser::parse(std::istream& in) {
    DrawRequest req;

    // 1. Путь к входному файлу
    auto inputPath = nextNonEmptyLine(in);
    if (!inputPath) return std::nullopt;
    req.inputPath = *inputPath;

    // 2. Начало отрезка
    auto startLine = nextNonEmptyLine(in);
    if (!startLine) return std::nullopt;
    auto start = parsePoint(*startLine);
    if (!start) return std::nullopt;
    req.start = *start;

    // 3. Конец отрезка
    auto endLine = nextNonEmptyLine(in);
    if (!endLine) return std::nullopt;
    auto end = parsePoint(*endLine);
    if (!end) return std::nullopt;
    req.end = *end;

    // 4. Цвет
    auto colorLine = nextNonEmptyLine(in);
    if (!colorLine) return std::nullopt;
    auto color = parseColor(*colorLine);
    if (!color) return std::nullopt;
    req.color = *color;

    // 5. Путь к выходному файлу
    auto outputPath = nextNonEmptyLine(in);
    if (!outputPath) return std::nullopt;
    req.outputPath = *outputPath;

    return req;
}

std::optional<int> Parser::parseInt(const std::string& s) {
    if (s.empty()) return std::nullopt;

    std::size_t pos = 0;
    int value = 0;
    try {
        value = std::stoi(s, &pos);
    } catch (const std::invalid_argument&) {
        return std::nullopt;
    } catch (const std::out_of_range&) {
        return std::nullopt;
    }

    while (pos < s.size() && std::isspace(static_cast<unsigned char>(s[pos])))
        ++pos;

    if (pos != s.size()) return std::nullopt;

    return value;
}

std::optional<Point> Parser::parsePoint(const std::string& s) {
    std::istringstream iss(s);
    std::string xStr, yStr;

    if (!(iss >> xStr >> yStr)) return std::nullopt;

    std::string extra;
    if (iss >> extra) return std::nullopt;

    auto x = parseInt(xStr);
    auto y = parseInt(yStr);
    if (!x || !y) return std::nullopt;

    return Point{*x, *y};
}

std::optional<Color> Parser::parseColor(const std::string& s) {
    std::istringstream iss(s);
    std::string rStr, gStr, bStr;

    if (!(iss >> rStr >> gStr >> bStr)) return std::nullopt;

    std::string extra;
    if (iss >> extra) return std::nullopt;

    auto r = parseInt(rStr);
    auto g = parseInt(gStr);
    auto b = parseInt(bStr);
    if (!r || !g || !b) return std::nullopt;

    return Color{*b, *g, *r};
}

std::optional<std::string> Parser::nextNonEmptyLine(std::istream& in) {
    std::string line;
    while (std::getline(in, line)) {
        // Проверить, что строка не пустая и не из одних пробелов
        if (line.find_first_not_of(" \t\r\n") != std::string::npos)
            return line;
    }
    return std::nullopt;
}

} // namespace mylib
