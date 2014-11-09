CPMF_PATH = ./

# rapidjson
RJSON_PATH = $(CPMF_PATH)/vendor/rapidjson
RJSON_INCLUDE_FLAGS = -I$(RJSON_PATH)/include
OBJ = matrix.o model.o

# cpmf
CPMF_INCLUDE_FLAGS = -I$(CPMF_PATH)

CXX = clang++
CFLAGS = -std=c++11 -funroll-loops -Wall
all: mf

%.o: cpmf/core/%.cpp cpmf/core/core.hpp
	$(CXX) $(CFLAGS) $(CPMF_INCLUDE_FLAGS) -c -o $@ $<

mf: cpmf/main.cpp $(OBJ) cpmf/core/core.hpp
	$(CXX) $(CFLAGS) $(RJSON_INCLUDE_FLAGS) $(CPMF_INCLUDE_FLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f mf $(OBJ)
