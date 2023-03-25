/**
 * Logic part of the program.
 * Path finding algorithm.
 *
 * Copyright (C) 2023 Saúl Valdelvira
 * License: MIT
 * Last update: 25-03-2022
*/
#include "heap.h"
#include <stdio.h>

static Node matrix[N_ROWS][N_COLS];

/**
 * Adds the node's neighbours to it's adjacency array.
*/
static void get_children(Node *node, int x, int y){
	int n_adj = 0;
	Node **adj = node->adjacency;
	if (x - 1 >= 0){
		adj[n_adj++] = &matrix[y][x-1];
		if (y + 1 < N_ROWS){
			adj[n_adj++] = &matrix[y+1][x-1];
		}
		if (y - 1 >= 0){
			adj[n_adj++] = &matrix[y-1][x-1];
		}
	}
	if (x + 1 < N_COLS){
		adj[n_adj++] = &matrix[y][x+1];
		if (y + 1 < N_ROWS){
			adj[n_adj++] = &matrix[y+1][x+1];
		}
		if (y - 1 >= 0){
			adj[n_adj++] = &matrix[y-1][x+1];
		}
	}
	if (y + 1 < N_ROWS){
		adj[n_adj++] = &matrix[y+1][x];
	}
	if (y - 1 >= 0){
		adj[n_adj++] = &matrix[y-1][x];
	}
	node->n_adjacencies = n_adj;
}

/**
 * Initializes the matrix.
*/
void init_matrix(){
	for (int i = 0; i < N_ROWS; ++i){
		for (int j = 0; j < N_COLS; ++j){
			matrix[i][j].parent = NULL;
			matrix[i][j].barrier = false;
			matrix[i][j].coord = (Coordinates) {.x = j, .y = i};
			get_children(&matrix[i][j], j, i);
		}
	}
}

/**
 * Heuristic for the A* algorithm.
 * It calculates the euclidean distance between the two
 * coordinates, but without the square root.
 * This saves computing time, and since we do the same with
 * all the nodes, the behaviour of the algorithm shouldn't be affected.
*/
static inline int heuristic(Coordinates c1, Coordinates c2){
	int a = c2.x - c1.x;
	int b = c2.y - c1.y;
	return a * a + b * b;
}

/**
 * Performs the A* path finding algorithm between the nodes
 * start and end.
 * It returns a Path structure, with an array of coordinates.
*/
Path find_path(Coordinates start, Coordinates end){
	for (int i = 0; i < N_ROWS; ++i){
		for (int j = 0; j < N_COLS; ++j){
			matrix[i][j].parent = NULL;
			matrix[i][j].visited = false;
			matrix[i][j].closed = false;
			matrix[i][j].heap_index = -1;
		}
	}

	Heap open = {.n_elements = 0};

	// Put the start node in the heap
	Node *start_node = &matrix[start.y][start.x];
	start_node->g = 0;
	start_node->h = 0;
	heap_add(&open, start_node);

	while (open.n_elements > 0){
		Node *current = heap_pop(&open);

		if (current->coord.x == end.x && current->coord.y == end.y){
			break;
		}

		current->visited = true;
		current->closed = true;

		int x = current->coord.x;
		int y = current->coord.y;

		for (int i = 0; i < current->n_adjacencies; ++i){
			Node *child = current->adjacency[i];
			if (child->barrier){
				continue;
			}

			int g = current->g + 1;
			int h = heuristic(child->coord, end);

			if (child->closed){
				if (g >= child->g){
					continue;
				}else{
					child->closed = false;
				}
			}

			bool exists = heap_exists(&open, child);
			if (!exists || g < child->g){
				if (exists){
					heap_change_priority(&open, child, g, h);
				}else{
					child->g = g;
					child->h = h;
					heap_add(&open, child);
				}
				child->parent = current;
			}

		}

	}
	// Trace back the path
	Path p = {.path_length = 0};
	Node *n = &matrix[end.y][end.x];
	do{
		p.path[p.path_length++] = n->coord;
		n = n->parent;
	}while(n);

	return p;
}

bool get_visited(Coordinates c){
	return matrix[c.y][c.x].visited;
}

void put_barrier(Coordinates c){
	matrix[c.y][c.x].barrier = !matrix[c.y][c.x].barrier;
}

bool get_barrier(Coordinates c){
	return matrix[c.y][c.x].barrier;
}

/**
 * Prepares a "maze template".
 * This means, it fills with barriers all the grid, except pa and pb,
 * which are the two points of the grid.
*/
void prepare_maze(Coordinates pa, Coordinates pb){
	for (int i = 0; i < N_ROWS; ++i){
		for (int j = 0; j < N_COLS; ++j){
			if ((j != pa.x || i != pa.y) && (j != pb.x || i != pb.y)){
				matrix[i][j].barrier = true;
			}
		}
	}
}

/**
 * Clears all the barriers of the grid.
*/
void clear_barriers(){
	for (int i = 0; i < N_ROWS; ++i){
		for (int j = 0; j < N_COLS; ++j){
			matrix[i][j].barrier = false;
		}
	}
}
