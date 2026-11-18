#include <gtest/gtest.h>
#include <stdexcept>
#include "StrategyRegistry.hpp"
#include "IdleStrategy.hpp"
#include "RandomStrategy.hpp"

TEST(StrategyRegistryTest, RegistersAndCreatesCustomFactory) {
    StrategyRegistry registry;
    registry.registerFactory("idle", [] {
        return std::make_unique<IdleStrategy>();
    });
    EXPECT_TRUE(registry.contains("idle"));
    auto strategy = registry.create("idle");
    ASSERT_NE(strategy, nullptr);
    EXPECT_EQ(strategy->name(), "idle");
}

TEST(StrategyRegistryTest, RejectsEmptyNameAndEmptyFactory) {
    StrategyRegistry registry;
    EXPECT_THROW(registry.registerFactory("", [] {
        return std::make_unique<IdleStrategy>();
    }), std::invalid_argument);
    EXPECT_THROW(registry.registerFactory("null", {}), std::invalid_argument);
}

TEST(StrategyRegistryTest, UnknownNameThrows) {
    const auto registry = StrategyRegistry::createDefault();
    EXPECT_THROW(registry.create("missing"), std::invalid_argument);
}

TEST(StrategyRegistryTest, DefaultRegistryContainsBuiltinsInSortedOrder) {
    const auto registry = StrategyRegistry::createDefault();
    const std::vector<std::string> expected{
        "distance", "greedy", "idle", "nearest", "random", "sweep"
    };
    EXPECT_EQ(registry.names(), expected);
    for (const auto& name : expected) EXPECT_TRUE(registry.contains(name));
}

TEST(StrategyRegistryTest, RegisteringExistingNameReplacesFactory) {
    StrategyRegistry registry;
    registry.registerFactory("agent", [] {
        return std::make_unique<IdleStrategy>();
    });
    registry.registerFactory("agent", [] {
        return std::make_unique<RandomStrategy>();
    });
    EXPECT_EQ(registry.create("agent")->name(), "random");
}
