// test_cli.cpp
#include <gtest/gtest.h>
#include "CliParser.hpp"
#include "StrategyRegistry.hpp"

namespace {

// Собирает argv из initializer_list<string>. argv[0] фиктивный.
AppConfig parseArgs(std::initializer_list<std::string> args,
                    const StrategyRegistry& reg = StrategyRegistry::createDefault()) {
    std::vector<std::string> storage;
    storage.reserve(args.size() + 1);
    storage.emplace_back("coins-test");
    storage.insert(storage.end(), args.begin(), args.end());

    std::vector<char*> argv;
    argv.reserve(storage.size());
    for (auto& s : storage) {
        // В C++17 std::string::data() возвращает char* (не const char*).
        argv.push_back(s.data());
    }

    CliParser parser;
    return parser.parse(static_cast<int>(argv.size()), argv.data(), reg);
}

} // namespace