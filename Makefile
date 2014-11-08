HOME = /Users/xxthermidorxx

CXX = clang++
CFLAGS = -std=c++11 -funroll-loops -Wall

# rapidjson
RJSON_PATH = $(HOME)/Github/cpmf/vendor/rapidjson
INCLUDE_FLAGS = -I$(RJSON_PATH)/include

all: cpmf

matrix.o: core/matrix.cpp core/core.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

cpmf: main.cpp matrix.o core/core.hpp
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f cpmf matrix.o
