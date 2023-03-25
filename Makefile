.PHONY: default clean

CC?=gcc
CCFLAGS= -lSDL2 -O3

default: path-finding.out

path-finding.out: src/main.o src/heap.o src/path_finding.o
	$(CC) -o path-finding.out src/main.o src/heap.o src/path_finding.o $(CCFLAGS)

.c.o:
	$(CC) -c $< -o $@ $(CCFLAGS)

clean:
	find . -name '*.o' -delete
	find . -name '*.out' -delete
	find . -name '*.exe' -delete
