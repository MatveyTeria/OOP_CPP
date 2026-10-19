CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude -IC:/msys64/ucrt64/include

LIB_SRCS := graph.cpp
LIB_OBJS := $(LIB_SRCS:.cpp=.o)

.PHONY: all test clean

all: app

app: app/main.o $(LIB_OBJS)
	$(CXX) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(LIB_OBJS) $(TEST_OBJS) app/main.o app run_tests