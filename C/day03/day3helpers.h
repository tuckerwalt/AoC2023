#ifndef DAY3HELPERS_H
#define DAY3HELPERS_H

#define ENGINE_DIMENSION 140

bool isSymbol(char c);
bool isGear(char (*engine)[ENGINE_DIMENSION + 2], int x, int y, int *n1x, int *n1y, int *n2x, int *n2y);
long getPartNum(char (*engine)[ENGINE_DIMENSION + 2], int x, int y);
bool checkAdjacentFirst(char (*engine)[ENGINE_DIMENSION + 2], int x, int y);
bool checkAdjacent(char (*engine)[ENGINE_DIMENSION + 2], int x, int y);
bool checkAdjacentLast(char (*engine)[ENGINE_DIMENSION + 2], int x, int y);

#endif