CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pthread
TARGET := thread_demo
SRCS := main.cc

all: $(TARGET)

$(TARGET): $(SRCS) Thread.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean

