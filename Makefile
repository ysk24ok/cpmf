HOME = /Users/xxthermidorxx

CXX = clang++
CFLAGS = -std=c++11 -funroll-loops -Wall

# rapidjson
RJSON_PATH = $(HOME)/Github/cpmf/vendor/rapidjson
INC_FLAGS = -I$(RJSON_PATH)/include

all: cpmf

cpmf: main.cpp
	$(CXX) $(CFLAGS) $(INC_FLAGS) -o $@ $<

clean:
	rm -f cpmf
