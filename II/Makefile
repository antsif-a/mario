CXX := clang++
CXXFLAGS := --std=c++23 -Wall -Wextra -lncurses -I. -O3 -ffast-math

all: main
	./$<

main: main.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

