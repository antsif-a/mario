CXX := clang++
CXXFLAGS := --std=c++23 -Wall -Wextra -lncurses

all: main
	./$<

main: main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

