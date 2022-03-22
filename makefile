
all: bench

bench:bench.cpp
	g++ -Wall -O3 bench.cpp -o $@ -g
