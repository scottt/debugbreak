CFLAGS := -Os -Wall -g
CXXFLAGS := $(CFLAGS)

PROGRAMS := $(basename $(wildcard *.c test/*.c test/*.cc *.S))

.PHONY: all clean
all: $(PROGRAMS)
clean:
	rm -f $(PROGRAMS) cscope.out tags

%: %.S
	$(CC) $(CFLAGS) -nostdlib $< -o $@

test/%: CPPFLAGS +=-I.
