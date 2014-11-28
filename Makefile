# It is assumed gcc with Cilk is on the $PATH
CXX := g++
CFLAGS := -O3 -std=c++11 -funroll-loops

# cpmf
CPMF_PATH = .
CPMF_INC_FLAGS = -I$(CPMF_PATH)
PARALLEL_FLAGS = -DTASK_PARALLEL_BASED
# PARALLEL_FLAGS = -DLINE_BASED
# PARALLEL_FLAGS = -DROTATION_BASED

# which task parallel library to use
ifeq ($(PARALLEL_FLAGS), -DTASK_PARALLEL_BASED)
	TP_FLAGS = -DTP_CILK
	# TP_FLAGS = -DTP_MYTH

	# add task-parallel dependent CFLAGS
	ifeq ($(TP_FLAGS), -DTP_CILK)
		DFLAGS := -fcilkplus -lcilkrts
	endif
	ifeq ($(TP_FLAGS), -DTP_MYTH)
		DFLAGS := -lmyth-native -ldr
	endif

else
	TP_FLAGS =
endif

# picojson
PICO_PATH = $(CPMF_PATH)/vendor/picojson
PICO_INC_FLAGS = -I$(PICO_PATH)

OBJ := matrix.o model.o tp_based_train.o timer.o logger.o

.PHONY: all clean
all: mf

%.o: cpmf/common/%.cpp
	$(CXX) $(CFLAGS) $(CPMF_INC_FLAGS) -c -o $@ $<

%.o: cpmf/utils/%.cpp
	$(CXX) $(CFLAGS) $(CPMF_INC_FLAGS) -c -o $@ $<

tp_based_train.o: cpmf/parallel/task_parallel_based/train.cpp
	$(CXX) $(CFLAGS) $(DFLAGS) $(CPMF_INC_FLAGS) $(TP_FLAGS) -c -o $@ $<

mf: cpmf/main.cpp $(OBJ)
	$(CXX) $(CFLAGS) $(DFLAGS) $(PICO_INC_FLAGS) $(CPMF_INC_FLAGS) $(PARALLEL_FLAGS) $(TP_FLAGS) -o $@ $<


clean:
	rm -f mf $(OBJ)
