/*
 * Path finding algorithm.
 * Author: Saúl Valdelvira (2023)
 */
#include "heap.h"
#include "path_finding.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

static Node *matrix;
#define matrix(i,j) matrix[(i) * n_cols + (j)]
extern int n_rows;
extern int n_cols;
Heap open;
static Path path;

// Defined in main.c, determines if the
// steps of the search must be rendered
extern bool animate_search;
bool horizontal_movement = true;
bool break_search;
void take_step();

#define abs(n) ((n) < 0 ? -(n) : (n))

/**
 * Adds the node's neighbours to it's adjacency array.
 */
static void get_children(Node *node, int x, int y){
	int n_adj = 0;
	Node **adj = node->adjacency;
	if (x - 1 >= 0){
		adj[n_adj++] = &matrix(y, x-1);
	}
	if (x + 1 < n_cols){
		adj[n_adj++] = &matrix(y,(x+1));
	}
	if (y + 1 < n_rows){
		adj[n_adj++] = &matrix(y+1,x);
	}
	if (y - 1 >= 0){
		adj[n_adj++] = &matrix((y-1) ,x);
	}
	if (horizontal_movement){
		if (x - 1 >= 0){
			if (y + 1 < n_rows){
				adj[n_adj++] = &matrix(y+1, x-1);
			}
			if (y - 1 >= 0){
				adj[n_adj++] = &matrix(y-1,x-1);
			}

		}
		if (x + 1 < n_cols){
			if (y + 1 < n_rows){
				adj[n_adj++] = &matrix(y+1,x+1);
			}
			if (y - 1 >= 0){
				adj[n_adj++] = &matrix(y-1,x+1);
			}
		}
	}
	node->n_adjacencies = n_adj;
}

/**
 * Initializes the matrix and the helper structures.
 */
int path_finding_init(){
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
		.path = malloc(sizeof(Coordinates) * n_rows * n_cols)
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

static inline double distance(Coordinates c1, Coordinates c2){
	if (horizontal_movement){
		return sqrt((c2.x - c1.x) * (c2.x - c1.x) + (c2.y - c1.y) * (c2.y - c1.y));
	}else{
		return abs(c2.x - c1.x) + abs(c2.y - c1.y);
	}
}

/**
 * Performs the A* path finding algorithm between the nodes
 * start and end.
 * It returns a Path structure, with an array of coordinates.
 */
Path find_path(Coordinates start, Coordinates end, heuristic_function heuristic){
	if (!heuristic){
		if (horizontal_movement){
			heuristic = heuristic_euclidean;
		}else{
			heuristic = heuristic_manhatan;
		}
	}
	break_search = false;
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
	start_node->g = 0.0;
	start_node->h = 0.0;
	heap_add(&open, start_node);

	Coordinates prev_coord = {0};

	while (open.n_elements > 0 && !break_search){
		Node *current = heap_pop(&open);
		if (current->coord.x == end.x && current->coord.y == end.y){
			break;
		}

		current->visited = true;
		current->closed = true;

		if (animate_search){
			take_step();
		}

		Coordinates diff1 = {
			.x = current->coord.x - prev_coord.x,
			.y = current->coord.y - prev_coord.y
		};

		for (int i = 0; i < current->n_adjacencies; ++i){
			Node *child = current->adjacency[i];
			if (child->barrier){
				continue;
			}

			double g = current->g + distance(current->coord, child->coord);
			double h = heuristic(child->coord, end);

			Coordinates diff2 = {
				.x = child->coord.x - current->coord.x,
				.y = child->coord.y - current->coord.y
			};

			// Slightly penalize changing direction
			bool turn = diff1.x != diff2.x || diff1.y != diff2.y;
			if (turn){
				h += 0.001;
			}

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
		prev_coord = current->coord;
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

void random_barriers(Coordinates pa, Coordinates pb){
	srand(time(NULL));
	for (int i = 0; i < n_rows; ++i){
		for (int j = 0; j < n_cols; ++j){
			if ((j != pa.x || i != pa.y) && (j != pb.x || i != pb.y)){
				int r = rand() % 100;
				if (r >= 60){
					matrix(i ,j).barrier = true;
				}else{
					matrix(i ,j).barrier = false;
				}
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

void set_break_search(){
	break_search = true;
}
