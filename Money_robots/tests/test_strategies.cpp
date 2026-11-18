#include <gtest/gtest.h>
#include <array>
#include "Board.hpp"
#include "Robot.hpp"
#include "GameState.hpp"
#include "IdleStrategy.hpp"
#include "RandomStrategy.hpp"
#include "GreedyDistanceStrategy.hpp"
#include "NearestCoinStrategy.hpp"

// ── Idle ──────────────────────────────────────────────────────────────────

TEST(IdleStrategy, AlwaysStaysEvenOnCoin) {
    Board b(3, 3);
    b.placeCoin({0, 0});
    std::vector<Robot> robots{Robot{"i", 0, {0, 0}}};
    // rng_value намеренно мусорный: idle не должен на него реагировать.
    GameState state{b, robots, 0, 0, 10, /*rng_value=*/123456};
    IdleStrategy s;
    for (int k = 0; k < 5; ++k) {
        EXPECT_EQ(s.chooseAction(state), Action::Stay);
    }
}

TEST(IdleStrategy, StaysOnEmptyBoard) {
    Board b(3, 3);
    std::vector<Robot> robots{Robot{"i", 0, {1, 1}}};
    GameState state{b, robots, 0, 0, 10, 0};
    IdleStrategy s;
    EXPECT_EQ(s.chooseAction(state), Action::Stay);
}

// ── Random ────────────────────────────────────────────────────────────────

TEST(RandomStrategy, AllActionsReachable) {
    // При равномерном распределении rng_value 0..N-1 все пять действий
    // должны встретиться. Это не строгий статистический тест, а smoke-test
    // контракта: «функция не возвращает фиксированное действие».
    Board b(3, 3);
    std::vector<Robot> robots{Robot{"r", 0, {1, 1}}};
    RandomStrategy s;

    std::array<bool, 5> seen{};
    for (std::uint32_t v = 0; v < 1000; ++v) {
        GameState state{b, robots, 0, 0, 10, v};
        seen[static_cast<int>(s.chooseAction(state))] = true;
    }
    for (std::size_t i = 0; i < seen.size(); ++i) {
        EXPECT_TRUE(seen[i]) << "action " << i << " never produced";
    }
}

TEST(RandomStrategy, SameRngValueSameAction) {
    Board b(3, 3);
    std::vector<Robot> robots{Robot{"r", 0, {1, 1}}};
    RandomStrategy s;

    for (std::uint32_t v : {0u, 1u, 42u, 100u, 0xFFFFFFFFu}) {
        GameState st1{b, robots, 0, 0, 10, v};
        GameState st2{b, robots, 0, 0, 10, v};
        EXPECT_EQ(s.chooseAction(st1), s.chooseAction(st2))
            << "rng_value=" << v;
    }
}

// ── GreedyDistance ────────────────────────────────────────────────────────

TEST(GreedyDistanceStrategy, MovesTowardNearestCoin) {
    Board b(5, 5);
    b.placeCoin({0, 0});
    std::vector<Robot> robots{Robot{"g", 0, {2, 2}}};
    GameState state{b, robots, 0, 0, 10, 0};
    GreedyDistanceStrategy s;
    // (0,0) — ближайшая по манхэттену; stepToward сначала решает row → Up.
    EXPECT_EQ(s.chooseAction(state), Action::Up);
}

TEST(GreedyDistanceStrategy, StaysOnCoin) {
    Board b(5, 5);
    b.placeCoin({1, 1});
    std::vector<Robot> robots{Robot{"g", 0, {1, 1}}};
    GameState state{b, robots, 0, 0, 10, 0};
    GreedyDistanceStrategy s;
    EXPECT_EQ(s.chooseAction(state), Action::Stay);
}

TEST(GreedyDistanceStrategy, StaysWithoutCoins) {
    Board b(3, 3);
    std::vector<Robot> robots{Robot{"g", 0, {1, 1}}};
    GameState state{b, robots, 0, 0, 10, 0};
    GreedyDistanceStrategy s;
    EXPECT_EQ(s.chooseAction(state), Action::Stay);
}

TEST(GreedyDistanceStrategy, DiffersFromNearestOnTieBreak) {
    // Робот в (2,2). Две монеты равноудалены: (4,2) и (2,4).
    // BFS (U/D/L/R): Down — первым в очереди, идём вниз.
    // Greedy (tie-break по row,col): (2,4) row=2 < 4 → идём вправо.
    // Тест кодирует это РАЗЛИЧИЕ — в этом смысл обеих стратегий в турнире.
    Board b(5, 5);
    b.placeCoin({4, 2});
    b.placeCoin({2, 4});
    std::vector<Robot> robots{Robot{"g", 0, {2, 2}}};
    GameState state{b, robots, 0, 0, 10, 0};

    NearestCoinStrategy n;
    GreedyDistanceStrategy g;
    EXPECT_EQ(n.chooseAction(state), Action::Down);
    EXPECT_EQ(g.chooseAction(state), Action::Right);
}

// ── Интеграция: Random в живой игре ──────────────────────────────────────

TEST(RandomStrategy, WorksInGameWithFixedSeed) {
    GameConfig cfg{5, 5, 3, 10, 42};
    std::vector<std::unique_ptr<IRobotStrategy>> s;
    s.push_back(std::make_unique<RandomStrategy>());
    Game g(cfg, std::move(s));
    EXPECT_NO_THROW(g.run());
}