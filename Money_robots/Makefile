# ── Toolchain ─────────────────────────────────────────────────────────────
CXX        := g++
CXXFLAGS   := -std=c++20 -Wall -Wextra -Wpedantic -g -O2 \
              -Isrc -Isrc/core -Isrc/strategies -Isrc/tournament \
              -Isrc/io -Isrc/cli -Iinclude \
              -IC:/msys64/ucrt64/include \
              -MMD -MP
LDFLAGS    := -LC:/msys64/ucrt64/lib
GTEST_LIBS := -lgtest_main -lgtest -pthread

# ── Library sources ───────────────────────────────────────────────────────
LIB_SRCS := \
	src/core/Action.cpp \
	src/core/Position.cpp \
	src/core/GameConfig.cpp \
	src/io/FastObserver.cpp \
    src/core/Board.cpp \
    src/core/Game.cpp \
    src/core/GameResult.cpp \
    src/core/Robot.cpp \
	src/strategies/IdleStrategy.cpp \
	src/strategies/RandomStrategy.cpp \
    src/strategies/NearestCoinStrategy.cpp \
    src/strategies/GreedyDistanceStrategy.cpp \
    src/strategies/SweepStrategy.cpp \
    src/strategies/GlobalGreedyStrategy.cpp \
    src/strategies/StrategyRegistry.cpp \
    src/tournament/Tournament.cpp \
    src/io/ConsoleBoardRenderer.cpp \
    src/io/DetailedObserver.cpp \
    src/cli/CliParser.cpp \
    src/cli/App.cpp

LIB_OBJS := $(LIB_SRCS:.cpp=.o)
LIB_DEPS := $(LIB_OBJS:.o=.d)

# ── Test sources ──────────────────────────────────────────────────────────
TEST_SRCS := \
    tests/test_position.cpp \
    tests/test_board.cpp \
    tests/test_robot.cpp \
    tests/test_strategies.cpp \
    tests/test_registry.cpp \
    tests/test_game.cpp \
    tests/test_tournament.cpp \
    tests/test_cli.cpp

TEST_OBJS := $(TEST_SRCS:.cpp=.o)
TEST_DEPS := $(TEST_OBJS:.o=.d)

# ── Application entry point ───────────────────────────────────────────────
APP_SRCS := src/main.cpp
APP_OBJS := $(APP_SRCS:.cpp=.o)
APP_DEPS := $(APP_OBJS:.o=.d)

# ── Environment for MSYS2/gtest ───────────────────────────────────────────
export TMPDIR := $(CURDIR)/tmp
export TMP    := $(CURDIR)/tmp
export TEMP   := $(CURDIR)/tmp

# ── Targets ───────────────────────────────────────────────────────────────
.PHONY: all test clean run_tests

all: coin_robots

coin_robots: $(APP_OBJS) $(LIB_OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

run_tests: $(TEST_OBJS) $(LIB_OBJS)
	$(CXX) $^ $(LDFLAGS) $(GTEST_LIBS) -o $@

test: run_tests
	./run_tests

%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(LIB_OBJS) $(LIB_DEPS) \
	      $(TEST_OBJS) $(TEST_DEPS) \
	      $(APP_OBJS)  $(APP_DEPS)
	rm -f coin_robots run_tests

-include $(LIB_DEPS) $(TEST_DEPS) $(APP_DEPS)