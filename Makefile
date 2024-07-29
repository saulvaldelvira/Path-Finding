.PHONY: default clean

CC ?= cc

CCFLAGS = -lSDL2 -lm -O3 -Wall -Wextra

CFILES = $(wildcard src/*.c)
OFILES = $(patsubst %.c,%.o,$(CFILES))

default: path-finding

path-finding: $(OFILES)
	@ $(CC) -o path-finding $(OFILES) $(CCFLAGS)

.c.o:
	@ echo " CC $@"
	@ $(CC) -c $< -o $@ $(CCFLAGS)

clean:
	@ find . -name '*.o' -delete
	@ find . -name '*.out' -delete
