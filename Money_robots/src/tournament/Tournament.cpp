#include "Tournament.hpp"

#include <algorithm>
#include <stdexcept>
#include <memory>

Tournament::Tournament(
    TournamentConfig config,
    const StrategyRegistry& registry)
    : config_(std::move(config)),
      registry_(registry) {

    if (config_.strategy_names.size() < 2) {
        throw std::invalid_argument(
            "Tournament requires at least 2 strategies");
    }

    if (config_.repetitions_per_pairing <= 0) {
        throw std::invalid_argument(
            "Repetitions must be positive");
    }
}

TournamentResult Tournament::run() {
    TournamentResult result;

    // Создаём статистику для каждой стратегии.
    for (const auto& name : config_.strategy_names) {
        result.stats.push_back({name, 0, 0, 0});
    }

    std::uint32_t game_number = 0;

    // Перебираем все уникальные пары стратегий.
    for (std::size_t i = 0;
         i < config_.strategy_names.size(); ++i) {

        for (std::size_t j = i + 1;
             j < config_.strategy_names.size(); ++j) {

            const std::string& name1 =
                config_.strategy_names[i];

            const std::string& name2 =
                config_.strategy_names[j];

            // Повторяем игру необходимое количество раз.
            for (int game = 0;
                 game < config_.repetitions_per_pairing;
                 ++game) {

                std::vector<std::unique_ptr<IRobotStrategy>>
                    strategies;

                strategies.push_back(registry_.create(name1));
                strategies.push_back(registry_.create(name2));

                GameConfig game_config =
                    config_.base_game_config;

                if (game_config.seed != 0) {
                    game_config.seed += ++game_number;
                }

                Game current_game(
                    game_config,
                    std::move(strategies));

                const GameResult game_result =
                    current_game.run();

                // Обновляем статистику.
                for (const auto& player : game_result.players) {
                    StrategyStats& stats =
                        result.stats[player.index == 0 ? i : j];

                    ++stats.games_played;
                    stats.total_score += player.score;
                }

                // Победителем считается первый игрок
                // в рейтинге с учётом всех критериев.
                const auto ranking = game_result.ranking();

                if (!ranking.empty()) {
                    const int winner = ranking.front();

                    if (winner == 0) {
                        ++result.stats[i].wins;
                    } else {
                        ++result.stats[j].wins;
                    }
                }
            }
        }
    }

    // Сортируем стратегии по количеству побед,
    // затем по сумме набранных очков.
    std::sort(
        result.stats.begin(),
        result.stats.end(),
        [](const StrategyStats& a, const StrategyStats& b) {
            if (a.wins != b.wins) {
                return a.wins > b.wins;
            }

            if (a.total_score != b.total_score) {
                return a.total_score > b.total_score;
            }

            return a.name < b.name;
        }
    );

    return result;
}