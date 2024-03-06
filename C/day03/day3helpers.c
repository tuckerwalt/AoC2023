
#include <stdbool.h>
#include "day3helpers.h"

bool isSymbol(char c)
{
	return !isdigit(c) && c != '.';
}

/**
 * Returns true if the given symbol is surrounded by exactly two part nums
 * n1x, n1y, n2x and n2y are filled with the x,y coordinates of the two part nums
*/
bool isGear(char (*engine)[ENGINE_DIMENSION + 2], int x, int y, int *n1x, int *n1y, int *n2x, int *n2y)
{
	int iFoundNums = 0;

	//check to the left
	if (isdigit(engine[x][y-1]))
	{
		*n1x = x;
		*n1y = y-1;
		iFoundNums++;
	}

	//check to the right
	if (isdigit(engine[x][y+1]))
	{
		if (iFoundNums == 0)
		{
			*n1x = x;
			*n1y = y+1;
		}
		else {
			*n2x = x;
			*n2y = y+1;
		}
		iFoundNums++;
	}

	//When checking above and below, two adjacent numbers means one part number
	bool bAPrevNum = false;
	bool bBPrevNum = false;

	for (int i = -1; i < 2; i++)
	{
		//above
		if (isdigit(engine[x-1][y+i]) && !bAPrevNum)
		{
			if (iFoundNums == 0)
			{
				*n1x = x-1;
				*n1y = y+i;
			}
			else if (iFoundNums == 1) 
			{
				*n2x = x-1;
				*n2y = y+i;
			}
			else
				return false;

			bAPrevNum = true;
			iFoundNums++;
		}
		else if (!isdigit(engine[x-1][y+i]))
		{
			bAPrevNum = false;
		}

		//below
		if (isdigit(engine[x+1][y+i]) && !bBPrevNum)
		{
			if (iFoundNums == 0)
			{
				*n1x = x+1;
				*n1y = y+i;
			}
			else if (iFoundNums == 1) 
			{
				*n2x = x+1;
				*n2y = y+i;
			}
			else
				return false;

			bBPrevNum = true;
			iFoundNums++;
		}
		else if (!isdigit(engine[x+1][y+i]))
		{
			bBPrevNum = false;
		}
	}

	if (iFoundNums == 2)
		return true;
	return false;
}

long getPartNum(char (*engine)[ENGINE_DIMENSION + 2], int x, int y)
{
	char szNum[5] = { 0 };
	int iNumLength = 0;
	int beginY = y;

	//find the beginning of the part num
	while (isdigit(engine[x][beginY - 1]))
		beginY--;
	
	while (isdigit(engine[x][beginY]))
		szNum[iNumLength++] = engine[x][beginY++];

	return atol(szNum);
}

/**
 * Look for symbols around the first, middle or last digit in a number
 * Returns true if any of the checked cells contain a symbol
*/
bool checkAdjacentFirst(char (*engine)[ENGINE_DIMENSION + 2], int x, int y)
{
	if (isSymbol(engine[x-1][y-1]) ||
		isSymbol(engine[x][y-1])   ||
		isSymbol(engine[x+1][y-1]) ||
		isSymbol(engine[x+1][y])   ||
		isSymbol(engine[x-1][y]))
	{
		return true;
	}

	return false;
}

bool checkAdjacent(char (*engine)[ENGINE_DIMENSION + 2], int x, int y)
{
	if (isSymbol(engine[x+1][y]) || isSymbol(engine[x-1][y]))
		return true;

	return false;
}

bool checkAdjacentLast(char (*engine)[ENGINE_DIMENSION + 2], int x, int y)
{
	if (isSymbol(engine[x][y])   ||
		isSymbol(engine[x+1][y]) ||
		isSymbol(engine[x-1][y]))
		return true;

	return false;
}