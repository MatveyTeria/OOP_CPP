#pragma once
#include <iosfwd>
#include "AppConfig.hpp"
#include "StrategyRegistry.hpp"

class CliParser {
public:
    // Парсит argv[1..argc-1]. Бросает std::invalid_argument при любой ошибке.
    // registry используется для валидации имён стратегий.
    [[nodiscard]] AppConfig parse(int argc, char** argv,
                                  const StrategyRegistry& registry) const;

    // Печатает usage-строку. static — чтобы App мог вызвать без экземпляра.
    static void printUsage(std::ostream& os);
};