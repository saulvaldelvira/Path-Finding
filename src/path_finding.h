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
#include "heuristic.h"

#define N_ROWS 45
#define N_COLS 80

typedef struct Path {
        Coordinates *path;
        int path_length;
} Path;

typedef struct Node{
	struct Node *parent;
	Coordinates coord;
	bool barrier;
	bool visited;
	bool closed;

	int heap_index;

	double g;
	double h;
	struct Node *adjacency[8];
	int n_adjacencies;
} Node;

void set_break_search();
Path find_path(Coordinates start, Coordinates end, heuristic_function heuristic);

void put_barrier(Coordinates c);
bool get_barrier(Coordinates c);

bool get_visited(Coordinates c);

int path_finding_init();
void path_finding_free();

void prepare_maze(Coordinates pa, Coordinates pb);
void clear_barriers();
void random_barriers(Coordinates pa, Coordinates pb);

void switch_horizontal_movement();

#endif
