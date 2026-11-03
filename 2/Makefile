CXX = g++
CXXFLAGS = -std=c++17 -O2 -static
TARGET = app.exe
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

export TMPDIR := $(CURDIR)/tmp
export TMP    := $(CURDIR)/tmp
export TEMP   := $(CURDIR)/tmp

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q *.o $(TARGET) 2>nul