#pragma once
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "IRobotStrategy.hpp"

class StrategyRegistry {
public:
    using Factory = std::function<std::unique_ptr<IRobotStrategy>()>;

    void registerFactory(const std::string& name, Factory f);
    [[nodiscard]] std::unique_ptr<IRobotStrategy> create(const std::string& name) const; // throws std::invalid_argument
    [[nodiscard]] std::vector<std::string> names() const;
    [[nodiscard]] bool contains(const std::string& name) const;

    // Регистрирует все встроенные стратегии
    static StrategyRegistry createDefault();

private:
    std::map<std::string, Factory> factories_;
};