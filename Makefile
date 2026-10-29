# GeometricDiodes kill-test build (single-threaded physics, std::thread over particle batches)
CXX      ?= clang++
CXXFLAGS ?= -std=c++20 -O2 -Wall -Wextra -Wpedantic -Isrc
LDFLAGS  ?= -lpthread

SRC  := $(wildcard src/*.cpp)
OBJ  := $(SRC:src/%.cpp=build/%.o)
APPS := build/gd_tests build/gd_killtest build/gd_geom

all: $(APPS)

build/%.o: src/%.cpp src/*.hpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/gd_%: apps/%.cpp $(OBJ) src/*.hpp | build
	$(CXX) $(CXXFLAGS) $< $(OBJ) -o $@ $(LDFLAGS)

build:
	mkdir -p build

test: build/gd_tests
	./build/gd_tests

clean:
	rm -rf build

.PHONY: all test clean
