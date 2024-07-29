#ifndef ARGS_H
#define ARGS_H

#include "heuristic.h"

extern int n_cols;
extern int n_rows;
extern int grid_cell_size;
extern int window_width;
extern int window_height;
extern heuristic_function heuristic;

void args_parse(int argc, char *argv[]);

#endif // ARGS_H
