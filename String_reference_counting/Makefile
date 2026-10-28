CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -IC:/msys64/ucrt64/include
GTEST_FLAGS := ...

LIB_SRCS := String.cpp
LIB_OBJS := $(LIB_SRCS:.cpp=.o)

TEST_SRCS := tests.cpp
TEST_OBJS := $(TEST_SRCS:.cpp=.o)

export TMPDIR := $(CURDIR)/tmp
export TMP    := $(CURDIR)/tmp
export TEMP   := $(CURDIR)/tmp

.PHONY: all test clean

all: app

app: main.o $(LIB_OBJS)
	$(CXX) $^ -o $@

run_tests: $(TEST_OBJS) $(LIB_OBJS)
	$(CXX) $^ -LC:/msys64/ucrt64/lib -lgtest -lgtest_main -o $@

test: run_tests
	./run_tests

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(LIB_OBJS) $(TEST_OBJS) main.o app run_tests