#include <stdio.h>
#include <stdlib.h>
#include "heuristic.h"
#include "path_finding.h"
#include <string.h>

#define CELL_WIDTH 20

int n_cols = N_COLS;
int n_rows = N_ROWS;
int grid_cell_size = CELL_WIDTH;
int window_width;
int window_height;

heuristic_function heuristic = NULL;

static void help(void);

void args_parse(int argc, char *argv[]) {
	for (int i = 1; i < argc; i++){
		if (argv[i][0] != '-'){
			continue;
		}
		switch (argv[i][1]){
		case 'd':
			n_rows = atoi(argv[i+1]);
			char *c = argv[i+1];
			while (*c != 'x' && *c != 'X' && *c != '\0'){
				c++;
			}
			if (*c == '\0'){
				fprintf(stderr, "Dimensions must be specified like so: <n_rows>x<n_cols>\n");
				return;
			}
			c++;
			n_cols = atoi(c);
			if (n_rows <= 0 || n_cols <= 0){
				fprintf(stderr, "Dimensions must be positive\n");
			}
			i++;
			break;
		case 'w':
			grid_cell_size = atoi(argv[i+1]);
			if (grid_cell_size <= 0){
				grid_cell_size = CELL_WIDTH;
			}
			break;
		case 'h':
			help();
			return; break;
		default:
			break;
		}
		if (argv[i][1] == '-'){
			if (strcmp(&argv[i][2], "heuristic") == 0){
				if (argc <= i+1){
					fprintf(stderr, "Available:\n"
						        "- blind (no heuristic, behaves like dijkstra)\n"
						        "- manhatan\n"
						        "- euclidean\n"
						        "- diagonal\n");
					exit(1);
				}
				if (strcmp(argv[++i], "blind") == 0){
					heuristic = heuristic_blind;
				}
				else if (strcmp(argv[i], "manhatan") == 0){
					heuristic = heuristic_manhatan;
				}
				else if (strcmp(argv[i], "euclidean") == 0){
					heuristic = heuristic_euclidean;
				}
				else if (strcmp(argv[i], "diagonal") == 0){
					heuristic = heuristic_diagonal;
				}
				else{
					fprintf(stderr, "Invalid argument to --heuristic: %s\n", argv[i]);
					exit(1);
				}
			}
			else if(strcmp(&argv[i][2], "help") == 0){
				help();
				exit(0);
			}
			else if(strcmp(&argv[i][2], "size") == 0){
				if (argc <= i+1){
					fprintf(stderr, "Available:\n"
						        "- small(30x30)\n"
						        "- medium(45x80). Default\n"
						        "- large(70x120)\n");
					exit(1);
				}
				if (strcmp(argv[++i], "small") == 0){
					n_rows = 30;
					n_cols = 30;
					grid_cell_size = 25;
				}
				else if (strcmp(argv[i], "medium") == 0){
					n_rows = 45;
					n_cols = 80;
				}
				else if (strcmp(argv[i], "large") == 0){
					n_rows = 85;
					n_cols = 150;
					grid_cell_size = 11;
				}else{
					fprintf(stderr, "Invalid argument to --size: %s\n", argv[i]);
					exit(1);
				}
			}
			else{
				fprintf(stderr, "Invalid option: %s\n", &argv[i][2]);
				exit(1);
			}

		}
	}
        return;
}

static void help(void) {
	printf(
		"[Path Finding]\n"
		"Ussage: path-finding -d [...] -w [...]\n"
		"Arguments:\n"
		"\t-d <n_rows>x<n_cols> : Set dimensions for the grid\n"
		"\t-w <width> : Set width of grid's cells\n"
		"\t--heuristic <name>: Set the heuristic to use\n"
		"\t--size [small|medium|large]: Set the size of the grid\n"
		"Keybindings:\n"
		"\t A: Display a search animation while traversing the grid\n"
		"\t V: Color the blocks which have been visited during the search.\n"
		"\t R: Generate a grid with random obstacles\n"
		"\t M: Fill all the grid with obstacles, so you can draw a maze.\n"
		"\t C: Clear the grid.\n"
		"\t T: Switch between dark and light theme\n"
		"\t H: Toggle horizontal movement on and off\n"
		"\t F5: Redraw path\n");
}
