# GeometricDiodes kill-test build (single-threaded physics, std::thread over particle batches)
CXX      ?= clang++
CXXFLAGS ?= -std=c++20 -O2 -Wall -Wextra -Wpedantic -Isrc -isystem /opt/homebrew/include/eigen3
LDFLAGS  ?= -lpthread

SRC  := $(wildcard src/*.cpp) $(wildcard src/hydro/*.cpp)
OBJ  := $(patsubst src/%.cpp,build/%.o,$(SRC))
APPS := build/gd_tests build/gd_killtest build/gd_geom build/gd_hydro_mesh build/gd_hydro_tests build/gd_hydro_sweep build/gd_hydro_fields build/gd_thz_tests build/gd_thz_sweep build/gd_thz_power

all: $(APPS)

.SECONDARY: $(OBJ)   # keep the objects between builds

build/%.o: src/%.cpp src/*.hpp src/hydro/*.hpp | build
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/gd_%: apps/%.cpp $(OBJ) src/*.hpp src/hydro/*.hpp apps/*.hpp | build
	$(CXX) $(CXXFLAGS) $< $(OBJ) -o $@ $(LDFLAGS)

build:
	mkdir -p build

test: build/gd_tests
	./build/gd_tests

hydro-test: build/gd_hydro_tests
	./build/gd_hydro_tests

venv:
	python3 -m venv .venv && .venv/bin/pip install -q -r requirements.txt

clean:
	rm -rf build

.PHONY: all test clean
