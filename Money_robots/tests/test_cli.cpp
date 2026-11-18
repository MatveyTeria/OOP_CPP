#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>

#include "App.hpp"
#include "CliParser.hpp"
#include "StrategyRegistry.hpp"

namespace {
AppConfig parseArgs(std::initializer_list<std::string> args,
                    const StrategyRegistry& registry = StrategyRegistry::createDefault()) {
    std::vector<std::string> storage{"coins-test"};
    storage.insert(storage.end(), args.begin(), args.end());
    std::vector<char*> argv;
    for (auto& item : storage) argv.push_back(item.data());
    CliParser parser;
    return parser.parse(static_cast<int>(argv.size()), argv.data(), registry);
}

int runApp(std::initializer_list<std::string> args,
           std::string* out_text = nullptr, std::string* err_text = nullptr) {
    std::vector<std::string> storage{"coins-test"};
    storage.insert(storage.end(), args.begin(), args.end());
    std::vector<char*> argv;
    for (auto& item : storage) argv.push_back(item.data());
    std::istringstream in;
    std::ostringstream out, err;
    App app(in, out, err);
    const int code = app.run(static_cast<int>(argv.size()), argv.data());
    if (out_text) *out_text = out.str();
    if (err_text) *err_text = err.str();
    return code;
}
}

TEST(CliParserTest, UsesFastDefaults) {
    const auto config = parseArgs({});
    EXPECT_EQ(config.mode, RunMode::Fast);
    EXPECT_EQ(config.rows, 10);
    EXPECT_EQ(config.cols, 10);
    EXPECT_EQ(config.num_coins, 20);
    EXPECT_EQ(config.max_turns, 50);
    EXPECT_EQ(config.games, 1);
    EXPECT_EQ(config.seed, 0u);
    EXPECT_EQ(config.strategies,
              (std::vector<std::string>{"random", "nearest", "greedy", "sweep"}));
}

TEST(CliParserTest, ParsesEqualsAndSeparateValueForms) {
    const auto config = parseArgs({"--mode", "detailed", "--rows=7",
                                   "--cols", "8", "--coins=9",
                                   "--turns", "11", "--seed=42",
                                   "--strategies=idle, random"});
    EXPECT_EQ(config.mode, RunMode::Detailed);
    EXPECT_EQ(config.rows, 7);
    EXPECT_EQ(config.cols, 8);
    EXPECT_EQ(config.num_coins, 9);
    EXPECT_EQ(config.max_turns, 11);
    EXPECT_EQ(config.seed, 42u);
    EXPECT_EQ(config.strategies, (std::vector<std::string>{"idle", "random"}));
}

TEST(CliParserTest, TournamentDefaultsToAllRegisteredStrategies) {
    const auto config = parseArgs({"--mode=tournament"});
    EXPECT_EQ(config.mode, RunMode::Tournament);
    EXPECT_EQ(config.strategies, StrategyRegistry::createDefault().names());
}

TEST(CliParserTest, HelpSkipsOtherValidation) {
    const auto config = parseArgs({"--help", "--rows=0"});
    EXPECT_TRUE(config.help_requested);
}

TEST(CliParserTest, RejectsUnknownAndMalformedArguments) {
    EXPECT_THROW(parseArgs({"--unknown=1"}), std::invalid_argument);
    EXPECT_THROW(parseArgs({"-x"}), std::invalid_argument);
    EXPECT_THROW(parseArgs({"--rows"}), std::invalid_argument);
    EXPECT_THROW(parseArgs({"--rows=12abc"}), std::invalid_argument);
    EXPECT_THROW(parseArgs({"--mode=invalid"}), std::invalid_argument);
}

TEST(CliParserTest, RejectsInvalidConfigurationAndStrategies) {
    EXPECT_THROW(parseArgs({"--rows=0"}), std::invalid_argument);
    EXPECT_THROW(parseArgs({"--cols=-1"}), std::invalid_argument);
    EXPECT_THROW(parseArgs({"--coins=-1"}), std::invalid_argument);
    EXPECT_THROW(parseArgs({"--rows=2", "--cols=2", "--coins=5"}), std::invalid_argument);
    EXPECT_THROW(parseArgs({"--turns=0"}), std::invalid_argument);
    EXPECT_THROW(parseArgs({"--games=0"}), std::invalid_argument);
    EXPECT_THROW(parseArgs({"--seed=-1"}), std::invalid_argument);
    EXPECT_THROW(parseArgs({"--strategies="}), std::invalid_argument);
    EXPECT_THROW(parseArgs({"--strategies=idle,,random"}), std::invalid_argument);
    EXPECT_THROW(parseArgs({"--strategies=missing"}), std::invalid_argument);
    EXPECT_THROW(parseArgs({"--mode=tournament", "--strategies=idle"}), std::invalid_argument);
}

TEST(AppTest, HelpReturnsZeroAndPrintsUsage) {
    std::string output;
    EXPECT_EQ(runApp({"--help"}, &output), 0);
    EXPECT_NE(output.find("Usage:"), std::string::npos);
}

TEST(AppTest, InvalidArgumentsReturnTwo) {
    std::string error;
    EXPECT_EQ(runApp({"--bad-option"}, nullptr, &error), 2);
    EXPECT_NE(error.find("Error:"), std::string::npos);
}

TEST(AppTest, FastModeRunsAndPrintsResults) {
    std::string output;
    EXPECT_EQ(runApp({"--mode=fast", "--strategies=idle", "--rows=2",
                      "--cols=2", "--coins=0", "--turns=1", "--seed=1"},
                     &output), 0);
    EXPECT_NE(output.find("Game started"), std::string::npos);
    EXPECT_NE(output.find("=== Results ==="), std::string::npos);
}


TEST(AppTest, DetailedModeCanRunWithEofInput) {
    std::string output;
    EXPECT_EQ(runApp({"--mode=detailed", "--strategies=idle", "--rows=2",
                      "--cols=2", "--coins=0", "--turns=1", "--seed=1"},
                     &output), 0);
    EXPECT_NE(output.find("=== Game started ==="), std::string::npos);
    EXPECT_NE(output.find("=== Final results ==="), std::string::npos);
}
