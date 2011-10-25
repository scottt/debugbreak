CFLAGS := -Os -Wall -g
CXXFLAGS := $(CFLAGS)

PROGRAMS := $(subst .c,,$(wildcard *.c)) $(subst .cc,,$(wildcard *.cc))

.PHONY: all clean
all: $(PROGRAMS)
thread-cancellation: LDFLAGS += -lpthread
c++-thread-cancellation: LDFLAGS += -lpthread
t: LDFLAGS += -lpthread
t0: LDFLAGS += -lpthread
clean:
	rm -f $(PROGRAMS)
