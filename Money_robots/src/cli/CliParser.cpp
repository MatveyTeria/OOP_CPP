#include "CliParser.hpp"

#include <array>
#include <cctype>
#include <charconv>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

// ── Утилиты ──────────────────────────────────────────────────────────────

std::string_view trim(std::string_view s) noexcept {
    // std::isspace ожидает unsigned char, приведённый к int.
    // Передача отрицательного char (например, кириллицы) — UB.
    auto is_sp = [](char ch) {
        return std::isspace(static_cast<unsigned char>(ch)) != 0;
    };
    while (!s.empty() && is_sp(s.front())) s.remove_prefix(1);
    while (!s.empty() && is_sp(s.back()))  s.remove_suffix(1);
    return s;
}

struct Arg {
    std::string_view key;
    std::optional<std::string_view> value;   // nullopt для формы `--key`
};

// `--mode=fast` → {mode, "fast"}; `--mode` → {mode, nullopt}; `-x` → throw.
Arg splitArg(std::string_view s) {
    if (s.size() < 3 || s[0] != '-' || s[1] != '-') {
        throw std::invalid_argument(
            "Expected --key[=value], got: '" + std::string(s) + "'");
    }
    const auto body = s.substr(2);
    const auto eq = body.find('=');
    if (eq == std::string_view::npos) return Arg{body, std::nullopt};
    return Arg{body.substr(0, eq), body.substr(eq + 1)};
}

// std::from_chars — локале-независимый, не бросает, парсит строго.
// std::stoi бы принял "42abc" → 42; здесь — ошибка.
int parseInt(std::string_view s, std::string_view name) {
    int out = 0;
    const auto* begin = s.data();
    const auto* end   = s.data() + s.size();
    const auto [ptr, ec] = std::from_chars(begin, end, out);
    if (ec != std::errc{} || ptr != end) {
        throw std::invalid_argument(
            "Invalid integer for --" + std::string(name) +
            ": '" + std::string(s) + "'");
    }
    return out;
}

RunMode parseMode(std::string_view s) {
    if (s == "detailed")   return RunMode::Detailed;
    if (s == "fast")       return RunMode::Fast;
    if (s == "tournament") return RunMode::Tournament;
    throw std::invalid_argument(
        "Unknown --mode: '" + std::string(s) +
        "' (expected detailed|fast|tournament)");
}

std::vector<std::string> parseStrategyList(std::string_view s,
                                           const StrategyRegistry& reg) {
    s = trim(s);
    if (s.empty()) throw std::invalid_argument("--strategies must not be empty");

    std::vector<std::string> result;
    std::size_t start = 0;
    while (start <= s.size()) {
        const auto comma = s.find(',', start);
        const auto end   = (comma == std::string_view::npos) ? s.size() : comma;
        const auto token = trim(s.substr(start, end - start));
        if (token.empty()) {
            throw std::invalid_argument(
                "Empty strategy name in --strategies");
        }
        const std::string name(token);
        if (!reg.contains(name)) {
            throw std::invalid_argument(
                "Unknown strategy: '" + name + "'");
        }
        result.push_back(name);
        if (comma == std::string_view::npos) break;
        start = end + 1;
    }
    return result;
}

std::vector<std::string> defaultStrategiesFor(RunMode mode,
                                              const StrategyRegistry& reg) {
    switch (mode) {
        case RunMode::Fast:
            return {"random", "nearest", "greedy", "sweep"};
        case RunMode::Detailed:
            return {"nearest"};
        case RunMode::Tournament:
            return reg.names();   // все зарегистрированные
    }
    return {};
}

void validateCount(std::size_t n, RunMode mode) {
    if (n == 0) {
        throw std::invalid_argument("At least one strategy required");
    }
    if (mode == RunMode::Tournament && n < 2) {
        throw std::invalid_argument(
            "Tournament requires at least 2 strategies");
    }
}

} // namespace

// ── Публичный API ────────────────────────────────────────────────────────

AppConfig CliParser::parse(int argc, char** argv,
                          const StrategyRegistry& registry) const {
    std::optional<RunMode>                         mode;
    std::optional<std::vector<std::string>>        strategies;
    std::optional<int>                             rows, cols, coins, turns, games;
    std::optional<std::uint32_t>                   seed;
    bool                                           help_requested = false;

    // Цикл по аргументам, начиная с argv[1] (argv[0] — имя программы).
    for (int i = 1; i < argc; ++i) {
        const Arg arg = splitArg(argv[i]);

        // takeValue достаёт значение: из `=value` либо из следующего argv.
        // Захват `i` по ссылке — иначе форма `--key value` не сдвинет цикл.
        auto takeValue = [&](std::string_view name) -> std::string_view {
            if (arg.value) return *arg.value;
            if (i + 1 >= argc) {
                throw std::invalid_argument(
                    "Missing value for --" + std::string(name));
            }
            return argv[++i];
        };

        if (arg.key == "help" || arg.key == "h") {
            help_requested = true;
            continue;
        }
        if (arg.key == "mode") {
            mode = parseMode(takeValue(arg.key));
        } else if (arg.key == "strategies") {
            strategies = parseStrategyList(takeValue(arg.key), registry);
        } else if (arg.key == "rows") {
            rows = parseInt(takeValue(arg.key), arg.key);
        } else if (arg.key == "cols") {
            cols = parseInt(takeValue(arg.key), arg.key);
        } else if (arg.key == "coins") {
            coins = parseInt(takeValue(arg.key), arg.key);
        } else if (arg.key == "turns") {
            turns = parseInt(takeValue(arg.key), arg.key);
        } else if (arg.key == "games") {
            games = parseInt(takeValue(arg.key), arg.key);
        } else if (arg.key == "seed") {
            const int v = parseInt(takeValue(arg.key), arg.key);
            if (v < 0) {
                throw std::invalid_argument("--seed must be >= 0");
            }
            seed = static_cast<std::uint32_t>(v);
        } else {
            throw std::invalid_argument(
                "Unknown argument: --" + std::string(arg.key));
        }
    }

    if (help_requested) {
        AppConfig cfg;
        cfg.help_requested = true;
        return cfg;
    }

    // Заполняем поля с учётом дефолтов.
    AppConfig cfg;
    cfg.mode        = mode.value_or(RunMode::Fast);
    cfg.rows        = rows.value_or(10);
    cfg.cols        = cols.value_or(10);
    cfg.num_coins   = coins.value_or(20);
    cfg.max_turns   = turns.value_or(50);
    cfg.games       = games.value_or(1);
    cfg.seed        = seed.value_or(0);
    cfg.strategies  = strategies.value_or(
        defaultStrategiesFor(cfg.mode, registry));

    // Финальная валидация — только то, что нельзя проверить по отдельности.
    if (cfg.rows <= 0 || cfg.cols <= 0) {
        throw std::invalid_argument("--rows and --cols must be positive");
    }
    if (cfg.num_coins < 0) {
        throw std::invalid_argument("--coins must be non-negative");
    }
    if (cfg.num_coins > cfg.rows * cfg.cols) {
        throw std::invalid_argument(
            "--coins (" + std::to_string(cfg.num_coins) +
            ") exceeds board size (" +
            std::to_string(cfg.rows * cfg.cols) + ")");
    }
    if (cfg.max_turns <= 0) {
        throw std::invalid_argument("--turns must be positive");
    }
    if (cfg.games <= 0) {
        throw std::invalid_argument("--games must be positive");
    }
    validateCount(cfg.strategies.size(), cfg.mode);
    return cfg;
}

void CliParser::printUsage(std::ostream& os) {
    os <<
        "Coin-Collecting Robots\n"
        "Usage: coins [options]\n"
        "\n"
        "Options:\n"
        "  --mode=MODE          detailed | fast | tournament  (default: fast)\n"
        "  --strategies=LIST    comma-separated strategy names\n"
        "  --rows=N             board rows          (default: 10)\n"
        "  --cols=N             board columns       (default: 10)\n"
        "  --coins=N            coins on board      (default: 20)\n"
        "  --turns=N            max turns           (default: 50)\n"
        "  --games=N            repetitions per pairing (tournament only)\n"
        "  --seed=N             0 = random, otherwise fixed (default: 0)\n"
        "  --help, -h           print this help\n";
}