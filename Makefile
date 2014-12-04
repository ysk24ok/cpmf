# It is assumed gcc with Cilk is on the $PATH
CXX := g++
CFLAGS := -O3 -std=c++11 -funroll-loops

# cpmf
CPMF_PATH = .
CPMF_INC_FLAGS = -I$(CPMF_PATH)
PARALLEL_FLAGS = -DTASK_PARALLEL_BASED
# PARALLEL_FLAGS = -DLINE_BASED
# PARALLEL_FLAGS = -DROTATION_BASED
# PARALLEL_FLAGS = -DFPSGD

# which task parallel library to use
ifeq ($(PARALLEL_FLAGS), -DTASK_PARALLEL_BASED)
	TP_FLAGS = -DTP_CILK
	# TP_FLAGS = -DTP_MYTH

	# add task-parallel dependent CFLAGS
	ifeq ($(TP_FLAGS), -DTP_CILK)
		DFLAGS := -fcilkplus -lcilkrts
		TP_INC_FLAGS =
		TP_LIB_FLAGS =
	endif
	ifeq ($(TP_FLAGS), -DTP_MYTH)
		DFLAGS := -lmyth-native -ldr
		MYTH_PATH = ./vendor/massivethreads
		TP_INC_FLAGS = -I$(MYTH_PATH)/include
		TP_LIB_FLAGS = -L$(MYTH_PATH)/lib -Wl,-R$(MYTH_PATH)/lib
	endif

else
	TP_FLAGS =
endif

# picojson
PICO_PATH = $(CPMF_PATH)/vendor/picojson
PICO_INC_FLAGS = -I$(PICO_PATH)

OBJ := matrix.o model.o train.o timer.o logger.o

.PHONY: all clean
all: mf

%.o: cpmf/common/%.cpp
	$(CXX) $(CFLAGS) $(CPMF_INC_FLAGS) -c -o $@ $<

%.o: cpmf/utils/%.cpp
	$(CXX) $(CFLAGS) $(CPMF_INC_FLAGS) -c -o $@ $<

# train.cpp for task_parallel_based
ifeq ($(PARALLEL_FLAGS), -DTASK_PARALLEL_BASED)
train.o: cpmf/parallel/task_parallel_based/train.cpp
	$(CXX) $(CFLAGS) $(DFLAGS) $(TP_INC_FLAGS) $(CPMF_INC_FLAGS) \
		$(TP_LIB_FLAGS) $(TP_FLAGS) -c -o $@ $<
endif
# train.cpp for line_based
ifeq ($(PARALLEL_FLAGS), -DLINE_BASED)
train.o: cpmf/parallel/line_based/train.cpp cpmf/parallel/line_based/scheduler.cpp
	$(CXX) $(CFLAGS) $(DFLAGS) $(CPMF_INC_FLAGS) -c -o $@ $<
endif
# train.cpp for fpsgd
ifeq ($(PARALLEL_FLAGS), -DFPSGD)
train.o: cpmf/parallel/fpsgd/train.cpp cpmf/parallel/fpsgd/scheduler.cpp
	$(CXX) $(CFLAGS) $(DFLAGS) $(CPMF_INC_FLAGS) -c -o $@ $<
endif

mf: cpmf/main.cpp $(OBJ)
	$(CXX) $(CFLAGS) $(DFLAGS) $(TP_INC_FLAGS) $(PICO_INC_FLAGS) \
		$(CPMF_INC_FLAGS) $(PARALLEL_FLAGS) $(TP_FLAGS) $(TP_LIB_FLAGS) -o $@ $<


clean:
	rm -f mf $(OBJ)
