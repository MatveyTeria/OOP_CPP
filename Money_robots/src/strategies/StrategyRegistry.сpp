#include "StrategyRegistry.hpp"

#include "GreedyDistanceStrategy.hpp"
#include "GlobalGreedyStrategy.hpp"
#include "IdleStrategy.hpp"
#include "NearestCoinStrategy.hpp"
#include "RandomStrategy.hpp"
#include "SweepStrategy.hpp"

#include <stdexcept>

void StrategyRegistry::registerFactory(const std::string& name, Factory f) {
    if (name.empty()) {
        throw std::invalid_argument("Strategy name must not be empty");
    }
    if (!f) {
        throw std::invalid_argument("Strategy factory must not be null");
    }
    factories_[name] = std::move(f);
}

std::unique_ptr<IRobotStrategy> StrategyRegistry::create(
        const std::string& name) const {
    const auto it = factories_.find(name);
    if (it == factories_.end()) {
        throw std::invalid_argument("Unknown strategy: '" + name + "'");
    }
    return it->second();   // Factory — std::function, вызывается как функция
}

std::vector<std::string> StrategyRegistry::names() const {
    // std::map хранит ключи в отсортированном порядке — значит, порядок
    // names() детерминирован без явного sort. Это удобно для --strategies
    // по умолчанию: они всегда в алфавитном порядке.
    std::vector<std::string> out;
    out.reserve(factories_.size());
    for (const auto& [name, _] : factories_) out.push_back(name);
    return out;
}

bool StrategyRegistry::contains(const std::string& name) const {
    return factories_.count(name) != 0;
}

StrategyRegistry StrategyRegistry::createDefault() {
    StrategyRegistry reg;
    reg.registerFactory("idle",    [] { return std::make_unique<IdleStrategy>(); });
    reg.registerFactory("random",  [] { return std::make_unique<RandomStrategy>(); });
    reg.registerFactory("nearest", [] { return std::make_unique<NearestCoinStrategy>(); });
    reg.registerFactory("greedy",  [] { return std::make_unique<GlobalGreedyStrategy>(); });
    reg.registerFactory("sweep",   [] { return std::make_unique<SweepStrategy>(); });
    reg.registerFactory("distance",[] { return std::make_unique<GreedyDistanceStrategy>(); });
    return reg;
}