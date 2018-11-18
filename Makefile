# It is assumed gcc with Cilk is on the $PATH
CXX := g++
CFLAGS := -O3 -std=c++11 -pthread -funroll-loops -Wall
DFLAGS =
CPMF_PATH := .
INCLUDE_FLAGS = -I$(CPMF_PATH)
LIBS =

# HERE, USERS HAVE TO DESIGNATE THE PARALLEL METHOD
# DPARALLEL := -DFPSGD
DPARALLEL := -DTP_BASED
	# which task parallel library to use
	DTP := -DTP_CILK
	# DTP := -DTP_MYTH


# for tp_based
ifeq ($(DPARALLEL), -DTP_BASED)

# for Cilk
ifeq ($(DTP), -DTP_CILK)
LIBS := -fcilkplus -lcilkrts
endif

# for MassiveThreads
ifeq ($(DTP), -DTP_MYTH)
MYTH_PATH = /usr/local
INCLUDE_FLAGS += -I$(MYTH_PATH)/include
LIBS := -L$(MYTH_PATH)/lib -Wl,-R$(MYTH_PATH)/lib -lmyth -ldr
endif

endif

# for picojson
PICO_PATH := $(CPMF_PATH)/vendor/picojson
PICO_INC_FLAGS := -I$(PICO_PATH)




DFLAGS += $(DPARALLEL)
DFLAGS += $(DTP)
OBJ := matrix.o model.o timer.o logger.o train.o

.PHONY: all clean
all: mf

%.o: cpmf/common/%.cpp cpmf/common/common.hpp
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

%.o: cpmf/utils/%.cpp cpmf/utils/utils.hpp
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

train.o: cpmf/parallel/train.cpp cpmf/parallel/fpsgd/fpsgd.hpp cpmf/parallel/tp_based/tp_based.hpp
	$(CXX) $(CFLAGS) $(DFLAGS) $(INCLUDE_FLAGS) -c -o $@ $< $(LIBS)

# for tp_based
ifeq ($(DPARALLEL), -DTP_BASED)
OBJ += scheduler.o
%.o: cpmf/parallel/tp_based/%.cpp cpmf/parallel/tp_based/tp_based.hpp cpmf/parallel/tp_based/tp_switch.hpp
	$(CXX) $(CFLAGS) $(DFLAGS) $(INCLUDE_FLAGS) -c -o $@ $< $(LIBS)
endif

# for fpsgd
ifeq ($(DPARALLEL), -DFPSGD)
OBJ += scheduler.o thread_pool.o
%.o: cpmf/parallel/fpsgd/%.cpp cpmf/parallel/fpsgd/fpsgd.hpp
	$(CXX) $(CFLAGS) $(DFLAGS) $(INCLUDE_FLAGS) -c -o $@ $< $(LIBS)
endif

mf: cpmf/main.cpp $(OBJ)
	$(CXX) $(CFLAGS) $(DFLAGS) $(PICO_INC_FLAGS) $(INCLUDE_FLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f mf $(OBJ)
