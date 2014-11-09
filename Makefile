HOME = /Users/xxthermidorxx

CXX = clang++
CFLAGS = -std=c++11 -funroll-loops -Wall

# rapidjson
RJSON_PATH = $(HOME)/Github/cpmf/vendor/rapidjson
RJSON_INCLUDE_FLAGS = -I$(RJSON_PATH)/include
OBJ = matrix.o model.o

all: cpmf

%.o: core/%.cpp core/core.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

cpmf: main.cpp $(OBJ) core/core.hpp
	$(CXX) $(CFLAGS) $(RJSON_INCLUDE_FLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f cpmf matrix.o
