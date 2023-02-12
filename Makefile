all: permprimes

DEBUG ?= 0

ifeq (${DEBUG}, 1)
  CXXFLAGS ?= -g -O0
else
  CXXFLAGS ?= -DNDEBUG=1 -Ofast -march=native
endif

permprimes: permprimes.cpp
	${CXX} ${CXXFLAGS} -Wall -Wextra -o $@ $< -lpthread

.PHONY: clean
clean:
	rm permprimes
