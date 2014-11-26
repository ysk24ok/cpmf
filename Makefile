# cpmf
CPMF_PATH = .
CPMF_INC_FLAGS = -I$(CPMF_PATH)
DFLAGS = "-DTASK_PARALLEL"

# picojson
PICO_PATH = $(CPMF_PATH)/vendor/picojson
PICO_INC_FLAGS = -I$(PICO_PATH)

# assuming gcc with Cilk is on the PATH
CXX := g++
CFLAGS := -O3 -std=c++11 -funroll-loops -fcilkplus -lcilkrts -Wall
OBJ := matrix.o model.o tp_based_train.o timer.o logger.o

all: mf

%.o: cpmf/common/%.cpp cpmf/common/common.hpp
	$(CXX) $(CFLAGS) $(CPMF_INC_FLAGS) -c -o $@ $<

%.o: cpmf/utils/%.cpp cpmf/utils/utils.hpp
	$(CXX) $(CFLAGS) $(CPMF_INC_FLAGS) -c -o $@ $<

tp_based_train.o: cpmf/parallel/task_parallel_based/train.cpp cpmf/common/common.hpp
	$(CXX) $(CFLAGS) $(CPMF_INC_FLAGS) $(CILK_INC_FLAGS) -c -o $@ $<

mf: cpmf/main.cpp $(OBJ) cpmf/common/common.hpp
	$(CXX) $(CFLAGS) $(PICO_INC_FLAGS) $(CPMF_INC_FLAGS) $(DFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f mf $(OBJ)
