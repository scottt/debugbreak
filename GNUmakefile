CFLAGS := -Os -Wall -g
CXXFLAGS := $(CFLAGS)

PROGRAMS := $(basename $(wildcard *.c test/*.c))

.PHONY: all clean
all: $(PROGRAMS)
clean:
	rm -f $(PROGRAMS) cscope.out tags

test/%: CFLAGS +=-I.
