#pragma once
#include <iosfwd>
#include <memory>
#include <iostream>
#include "AppConfig.hpp"
#include "StrategyRegistry.hpp"
#include "IGameObserver.hpp"

class App {
public:
    // Внедряем потоки — это делает App тестируемым без подмены cin/cout.
    // По умолчанию — std::cin/std::cout/std::cerr.
    explicit App(std::istream& in  = std::cin,
                 std::ostream& out = std::cout,
                 std::ostream& err = std::cerr);

    // Возвращает код выхода: 0 — успех, 1 — runtime error, 2 — bad usage.
    [[nodiscard]] int run(int argc, char** argv);

private:
    int runDetailed(const AppConfig& cfg, const StrategyRegistry& reg);
    int runFast(const AppConfig& cfg, const StrategyRegistry& reg);
    int runTournament(const AppConfig& cfg, const StrategyRegistry& reg);

    std::istream& in_;
    std::ostream& out_;
    std::ostream& err_;
};