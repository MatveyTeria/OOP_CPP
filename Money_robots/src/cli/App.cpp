#include "App.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "CliParser.hpp"
#include "ConsoleBoardRenderer.hpp"
#include "DetailedObserver.hpp"
#include "FastObserver.hpp"
#include "Tournament.hpp"
#include "Game.hpp"

App::App(std::istream& in, std::ostream& out, std::ostream& err)
    : in_(in), out_(out), err_(err) {}

int App::run(int argc, char** argv) {
    // Каждый запуск — свежий реестр. Это дешево (6 лямбд),
    // но защищает от «глобального состояния» между вызовами run().
    const StrategyRegistry registry = StrategyRegistry::createDefault();

    AppConfig cfg;
    try {
        CliParser parser;
        cfg = parser.parse(argc, argv, registry);
    } catch (const std::invalid_argument& e) {
        err_ << "Error: " << e.what() << "\n\n";
        CliParser::printUsage(err_);
        return 2;
    }

    if (cfg.help_requested) {
        CliParser::printUsage(out_);
        return 0;
    }

    try {
        switch (cfg.mode) {
            case RunMode::Detailed:   return runDetailed(cfg, registry);
            case RunMode::Fast:       return runFast(cfg, registry);
            case RunMode::Tournament: return runTournament(cfg, registry);
        }
    } catch (const std::exception& e) {
        err_ << "Runtime error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}

// ── fast: одна игра, N ходов, M монет ────────────────────────────────────

int App::runFast(const AppConfig& cfg, const StrategyRegistry& reg) {
    std::vector<std::unique_ptr<IRobotStrategy>> strategies;
    strategies.reserve(cfg.strategies.size());
    for (const auto& name : cfg.strategies) {
        strategies.push_back(reg.create(name));
    }

    GameConfig gcfg{cfg.rows, cfg.cols, cfg.num_coins, cfg.max_turns, cfg.seed};

    auto observer = std::make_shared<FastObserver>(out_);
    Game game(gcfg, std::move(strategies), observer);
    game.run();
    return 0;
}

// ── detailed: та же игра, но с пошаговым выводом и ожиданием Enter ────────

int App::runDetailed(const AppConfig& cfg, const StrategyRegistry& reg) {
    std::vector<std::unique_ptr<IRobotStrategy>> strategies;
    for (const auto& name : cfg.strategies) {
        strategies.push_back(reg.create(name));
    }

    GameConfig gcfg{cfg.rows, cfg.cols, cfg.num_coins, cfg.max_turns, cfg.seed};

    auto observer = std::make_shared<DetailedObserver>(in_, out_);
    Game game(gcfg, std::move(strategies), observer);
    game.run();
    return 0;
}

// ── tournament: серия парных игр ─────────────────────────────────────────

int App::runTournament(const AppConfig& cfg, const StrategyRegistry& reg) {
    TournamentConfig tcfg;
    tcfg.strategy_names = cfg.strategies;
    tcfg.base_game_config = GameConfig{
        cfg.rows, cfg.cols, cfg.num_coins, cfg.max_turns, cfg.seed};
    tcfg.repetitions_per_pairing = cfg.games;

    Tournament t(tcfg, reg);
    const TournamentResult result = t.run();

    out_ << "\n=== Tournament results ===\n";
    out_ << "  #  strategy       games   wins   total_score\n";
    int rank = 1;
    for (const auto& s : result.stats) {
        out_ << "  " << rank++ << "  "
             << s.name;
        for (std::size_t i = s.name.size(); i < 14; ++i) out_ << ' ';
        out_ << s.games_played << "      "
             << s.wins << "      "
             << s.total_score << '\n';
    }
    return 0;
}