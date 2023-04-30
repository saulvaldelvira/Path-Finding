#include "heuristic.h"

#include <math.h>
#define abs(n) ((n) < 0 ? -(n) : (n))

double heuristic_manhatan(Coordinates c1, Coordinates c2){
	int delt_x = abs(c1.x - c2.x); 
	int delt_y = abs(c1.y - c2.y);
	return delt_x + delt_y;
}

double heuristic_euclidean(Coordinates c1, Coordinates c2){
	int delt_x = abs(c1.x - c2.x); 
	int delt_y = abs(c1.y - c2.y);
	return sqrt(delt_x * delt_x + delt_y * delt_y);
}

double heuristic_blind(Coordinates c1, Coordinates c2){
	(void) c1;
	(void) c2;
	return 0;
}
