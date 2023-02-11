all: permprimes

permprimes: permprimes.cpp
	g++ -O3 -Wall -Wextra -o $@ $< -lpthread

.PHONY: clean
clean:
	rm permprimes
