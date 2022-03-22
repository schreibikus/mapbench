
all: bench

bench:bench.cpp
	g++ -O2 bench.cpp -o $@
