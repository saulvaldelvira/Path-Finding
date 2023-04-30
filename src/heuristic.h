#ifndef _HEURISTIC_H
#define _HEURISTIC_H

typedef struct Coordinates {
	int x;
	int y;
} Coordinates;

typedef double (*heuristic_function)(Coordinates, Coordinates);

double heuristic_manhatan(Coordinates c1, Coordinates c2);

double heuristic_euclidean(Coordinates c1, Coordinates c2);

double heuristic_blind(Coordinates c1, Coordinates c2);

#endif // _HEURISTIC_H
