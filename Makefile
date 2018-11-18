# It is assumed gcc with Cilk is on the $PATH
CXX := g++
CFLAGS := -O3 -std=c++11 -funroll-loops -Wall
DFLAGS =
CPMF_PATH := .
INCLUDE_FLAGS = -I$(CPMF_PATH)
LIB_FLAGS =

# HERE, USERS HAVE TO DESIGNATE THE PARALLEL METHOD
# DPARALLEL := -DFPSGD
DPARALLEL := -DTP_BASED
	# which task parallel library to use
	DTP := -DTP_CILK
	# DTP := -DTP_MYTH


# for fpsgd
TP_FLAGS := -lpthread

# for Cilk
ifeq ($(DTP), -DTP_CILK)
TP_FLAGS := -fcilkplus -lcilkrts
endif

# for MassiveThreads
ifeq ($(DTP), -DTP_MYTH)
TP_FLAGS := -lmyth-native -ldr
MYTH_PATH = $(CPMF_PATH)/vendor/massivethreads
INCLUDE_FLAGS += -I$(MYTH_PATH)/include
LIB_FLAGS += -L$(MYTH_PATH)/lib -Wl,-R$(MYTH_PATH)/lib
endif

# for picojson
PICO_PATH := $(CPMF_PATH)/vendor/picojson
PICO_INC_FLAGS := -I$(PICO_PATH)




DFLAGS += $(DPARALLEL)
DFLAGS += $(DTP)
INCLUDE_FLAGS += $(LIB_FLAGS)
OBJ := matrix.o model.o timer.o logger.o train.o

.PHONY: all clean
all: mf

%.o: cpmf/common/%.cpp cpmf/common/common.hpp
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

%.o: cpmf/utils/%.cpp cpmf/utils/utils.hpp
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

train.o: cpmf/parallel/train.cpp cpmf/parallel/fpsgd/fpsgd.hpp cpmf/parallel/tp_based/tp_based.hpp
	$(CXX) $(CFLAGS) $(TP_FLAGS) $(DFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# for tp_based
ifeq ($(DPARALLEL), -DTP_BASED)
OBJ += scheduler.o
%.o: cpmf/parallel/tp_based/%.cpp cpmf/parallel/tp_based/tp_based.hpp cpmf/parallel/tp_based/tp_switch.hpp
	$(CXX) $(CFLAGS) $(TP_FLAGS) $(DFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<
endif

# for fpsgd
ifeq ($(DPARALLEL), -DFPSGD)
OBJ += scheduler.o thread_pool.o
%.o: cpmf/parallel/fpsgd/%.cpp cpmf/parallel/fpsgd/fpsgd.hpp
	$(CXX) $(CFLAGS) $(TP_FLAGS) $(DFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<
endif

mf: cpmf/main.cpp $(OBJ)
	$(CXX) $(CFLAGS) $(TP_FLAGS) $(DFLAGS) $(PICO_INC_FLAGS) $(INCLUDE_FLAGS) -o $@ $^

clean:
	rm -f mf $(OBJ)
