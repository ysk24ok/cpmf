# cpmf
CPMF_PATH = .
CPMF_INCLUDE_FLAGS = -I$(CPMF_PATH)
DFLAGS = "-DTASK_PARALLEL"

# rapidjson
RJSON_PATH = $(CPMF_PATH)/vendor/rapidjson
RJSON_INCLUDE_FLAGS = -I$(RJSON_PATH)/include

# assuming gcc with Cilk is on the PATH
CXX := g++
CFLAGS := -std=c++11 -funroll-loops -fcilkplus -lcilkrts -Wall
OBJ := matrix.o model.o tp_based_train.o timer.o

all: mf

%.o: cpmf/common/%.cpp cpmf/common/common.hpp
	$(CXX) $(CFLAGS) $(CPMF_INCLUDE_FLAGS) -c -o $@ $<

%.o: cpmf/utils/%.cpp cpmf/utils/utils.hpp
	$(CXX) $(CFLAGS) $(CPMF_INCLUDE_FLAGS) -c -o $@ $<

tp_based_train.o: cpmf/parallel/task_parallel_based/train.cpp cpmf/common/common.hpp
	$(CXX) $(CFLAGS) $(CPMF_INCLUDE_FLAGS) $(CILK_INCLUDE_FLAGS) -c -o $@ $<

mf: cpmf/main.cpp $(OBJ) cpmf/common/common.hpp
	$(CXX) $(CFLAGS) $(RJSON_INCLUDE_FLAGS) $(CPMF_INCLUDE_FLAGS) $(DFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f mf $(OBJ)
