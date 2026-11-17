#include "Game.hpp"

#include <algorithm>
#include <numeric>
#include <sstream>
#include <stdexcept>

Game::Game(GameConfig cfg,
           std::vector<std::unique_ptr<IRobotStrategy>> strategies,
           std::shared_ptr<IGameObserver> observer)
    : cfg_(std::move(cfg)),
      board_(cfg_.rows, cfg_.cols),
      strategies_(std::move(strategies)),
      observer_(std::move(observer)),
      rng_(cfg_.seed == 0 ? std::random_device{}() : cfg_.seed)
{
    cfg_.validate();
    if (strategies_.empty()) {
        throw std::invalid_argument("Game: at least one strategy required");
    }
    if (cfg_.num_coins > cfg_.rows * cfg_.cols) {
        throw std::invalid_argument("Game: too many coins for board size");
    }
}

void Game::initializeBoard() {
    const int total_cells = cfg_.rows * cfg_.cols;

    // 1. Равномерная выборка без повторов клеток — тасование пула индексов.
    //    В Python — random.sample(range(total_cells), num_coins).
    std::vector<int> pool(total_cells);
    std::iota(pool.begin(), pool.end(), 0);          // 0..total_cells-1
    std::shuffle(pool.begin(), pool.end(), rng_);

    for (int i = 0; i < cfg_.num_coins; ++i) {
        const int flat = pool[i];
        board_.placeCoin({flat / cfg_.cols, flat % cfg_.cols});
    }
}

void Game::initializeRobots() {
    robots_.reserve(strategies_.size());
    for (std::size_t i = 0; i < strategies_.size(); ++i) {
        const int r = std::uniform_int_distribution<int>(0, cfg_.rows - 1)(rng_);
        const int c = std::uniform_int_distribution<int>(0, cfg_.cols - 1)(rng_);
        robots_.emplace_back(strategies_[i]->name(), static_cast<int>(i),
                             Position{r, c});
    }
}

GameResult Game::run() {
    initializeBoard();
    initializeRobots();

    if (observer_) observer_->onGameStart(board_, robots_);

    for (int turn = 0; turn < cfg_.max_turns; ++turn) {
        if (observer_) observer_->onTurnStart(turn, robots_);

        // 1. Снимок состояния ОДИН раз — все стратегии видят идентичные данные.
        //    GameState содержит const-ссылки; менять состояние из стратегии нельзя.
        std::vector<Action> actions;
        actions.reserve(robots_.size());
        for (std::size_t i = 0; i < robots_.size(); ++i) {
            const GameState state{board_, robots_, static_cast<int>(i),
                                  turn, cfg_.max_turns};
            actions.push_back(strategies_[i]->chooseAction(state));
        }

        // 2. Применяем
        applyActions(actions, turn);

        if (observer_) observer_->onTurnEnd(turn, actions, board_, robots_);

        // 3. Ранний выход: монет не осталось
        if (board_.remainingCoins() == 0) break;
    }

    GameResult result = buildResult();
    if (observer_) observer_->onGameEnd(result);
    return result;
}

void Game::applyActions(const std::vector<Action>& actions, int turn) {
    // Внутренний инвариант: размер actions совпадает с числом роботов.
    if (actions.size() != robots_.size()) {
        throw std::logic_error("Game: actions/robots size mismatch");
    }

    // Шаг 1. Вычисляем намерения ДО применения — чтобы все стратегии
    //         действительно «ходили одновременно».
    std::vector<Position> intended(robots_.size());
    for (std::size_t i = 0; i < robots_.size(); ++i) {
        const auto [dr, dc] = deltaOf(actions[i]);
        const Position cur = robots_[i].position();
        const Position next{cur.row + dr, cur.col + dc};
        intended[i] = next.isInside(board_.rows(), board_.cols()) ? next : cur;
    }

    // Шаг 2. Применяем последовательно: двигаем и собираем монеты.
    //         Порядок — по индексу робота → монета достаётся «меньшему».
    for (std::size_t i = 0; i < robots_.size(); ++i) {
        robots_[i].moveTo(intended[i]);
        if (board_.takeCoin(intended[i])) {
            robots_[i].collectCoin(turn);
        }
    }
}