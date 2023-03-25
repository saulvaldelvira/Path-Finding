/**
 * Logic part of the program.
 * Path finding algorithm.
 *
 * Copyright (C) 2023 Saúl Valdelvira
 * License: MIT
 * Last update: 25-03-2022
*/
#ifndef PATH_FINDING_H
#define PATH_FINDING_H

#include <stdbool.h>

#define N_ROWS 60
#define N_COLS 100

typedef struct Coordinates {
	int x;
	int y;
} Coordinates;

typedef struct Path {
        Coordinates path[N_ROWS * N_COLS];
        int path_length;
} Path;

typedef struct Node{
	struct Node *parent;
	Coordinates coord;
	bool barrier;
	bool visited;
	bool closed;

	int heap_index;

	int g;
	int h;
	struct Node *adjacency[8];
	int n_adjacencies;
} Node;

Path find_path(Coordinates start, Coordinates end);

void put_barrier(Coordinates c);
bool get_barrier(Coordinates c);

bool get_visited(Coordinates c);

void init_matrix();

void prepare_maze(Coordinates pa, Coordinates pb);
void clear_barriers();

#endif
