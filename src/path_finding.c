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
#include <stdlib.h>

static Node *matrix;
#define matrix(i,j) matrix[(i) * n_cols + (j)]
int n_rows;
int n_cols;
Heap open;
Path path;

bool horizontal_movement = true;

/**
 * Adds the node's neighbours to it's adjacency array.
*/
static void get_children(Node *node, int x, int y){
	int n_adj = 0;
	Node **adj = node->adjacency;
	if (x - 1 >= 0){
		adj[n_adj++] = &matrix(y, x-1);
		if (horizontal_movement){
			if (y + 1 < n_rows){
			adj[n_adj++] = &matrix(y+1, x-1);
			}
			if (y - 1 >= 0){
				adj[n_adj++] = &matrix(y-1,x-1);
			}
		}
	}
	if (x + 1 < n_cols){
		adj[n_adj++] = &matrix(y,(x+1));
		if (horizontal_movement){
			if (y + 1 < n_rows){
			adj[n_adj++] = &matrix(y+1,x+1);
			}
			if (y - 1 >= 0){
				adj[n_adj++] = &matrix(y-1,x+1);
			}
		}
	}
	if (y + 1 < n_rows){
		adj[n_adj++] = &matrix(y+1,x);
	}
	if (y - 1 >= 0){
		adj[n_adj++] = &matrix((y-1) ,x);
	}
	node->n_adjacencies = n_adj;
}

/**
 * Initializes the matrix and the helper structures.
*/
int path_finding_init(int rows, int cols){
	n_rows = rows;
	n_cols = cols;
	matrix = malloc(n_rows * n_cols * sizeof(*matrix));
	if (!matrix){
		return -1;
	}
	for (int i = 0; i < n_rows; i++){
		for (int j = 0; j < n_cols; j++){
			matrix(i ,j).parent = NULL;
			matrix(i ,j).barrier = false;
			matrix(i ,j).coord = (Coordinates) {.x = j, .y = i};
			get_children(&matrix(i ,j), j, i);
		}
	}
	open = (Heap){
		.n_elements = 0,
		.elements = malloc(sizeof(Node*) * n_rows * n_cols)
	};
	if (!open.elements){
		return -1;
	}
	path = (Path){
		.path_length = 0,
		.path = malloc(sizeof(Coordinates*) * n_rows * n_cols)
	};
	if (!path.path){
		return -1;
	}
	return 1;
}

void path_finding_free(){
	free(matrix);
	free(open.elements);
	free(path.path);
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
	for (int i = 0; i < n_rows; i++){
		for (int j = 0; j < n_cols; j++){
			matrix(i ,j).parent = NULL;
			matrix(i ,j).visited = false;
			matrix(i ,j).closed = false;
			matrix(i ,j).heap_index = -1;
		}
	}
	open.n_elements = 0;

	// Put the start node in the heap
	Node *start_node = &matrix(start.y ,start.x);
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
	path.path_length = 0;
	Node *n = &matrix(end.y ,end.x);
	do{
		path.path[path.path_length++] = n->coord;
		n = n->parent;
	}while(n);

	return path;
}

bool get_visited(Coordinates c){
	return matrix(c.y ,c.x).visited;
}

void put_barrier(Coordinates c){
	matrix(c.y, c.x).barrier = !matrix(c.y, c.x).barrier;
}

bool get_barrier(Coordinates c){
	return matrix(c.y ,c.x).barrier;
}

/**
 * Prepares a "maze template".
 * This means, it fills with barriers all the grid, except pa and pb,
 * which are the two points of the grid.
*/
void prepare_maze(Coordinates pa, Coordinates pb){
	for (int i = 0; i < n_rows; ++i){
		for (int j = 0; j < n_cols; ++j){
			if ((j != pa.x || i != pa.y) && (j != pb.x || i != pb.y)){
				matrix(i ,j).barrier = true;
			}
		}
	}
}

/**
 * Clears all the barriers of the grid.
*/
void clear_barriers(){
	for (int i = 0; i < n_rows; ++i){
		for (int j = 0; j < n_cols; ++j){
			matrix(i ,j).barrier = false;
		}
	}
}

void switch_horizontal_movement(){
	horizontal_movement = !horizontal_movement;
	for (int i = 0; i < n_rows; i++){
		for (int j = 0; j < n_cols; j++){
			get_children(&matrix(i ,j), j, i);
		}
	}
}
