.PHONY: default clean

CC ?=gcc
CCFLAGS = -lSDL2 -lm -O3 -Wall -Wextra

CFILES = $(wildcard src/*.c)
OFILES = $(patsubst %.c,%.o,$(CFILES))

default: path-finding.out

path-finding.out: $(OFILES)
	$(CC) -o path-finding.out $(OFILES) $(CCFLAGS)

.c.o:
	$(CC) -c $< -o $@ $(CCFLAGS)

clean:
	find . -name '*.o' -delete
	find . -name '*.out' -delete
