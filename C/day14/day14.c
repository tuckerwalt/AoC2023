#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <time.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 150

int rollNorth(char **map, int x, int y, int iDim)
{
	int iNewX = x - 1;

	while(iNewX >= 0)
	{
		if ('#' == map[iNewX][y] || 'O' == map[iNewX][y])
			break;

		iNewX--;
	}

	return iNewX + 1;
}

void tiltNorth(char **map, int iDim)
{
	for (int y = 0; y < iDim; y++)
	{
		for (int x = 0; x < iDim; x++)
		{
			if ('O' == map[x][y])
			{
				int iNewX = rollNorth(map, x, y, iDim);
				if (iNewX != x)
				{
					map[iNewX][y] = 'O';
					map[x][y] = '.';
				}
			}
		}
	}
}

int rollSouth(char **map, int x, int y, int iDim)
{
	int iNewX = x + 1;

	while(iNewX < iDim)
	{
		if ('#' == map[iNewX][y] || 'O' == map[iNewX][y])
			break;

		iNewX++;
	}

	return iNewX - 1;
}

void tiltSouth(char **map, int iDim)
{
	for (int y = 0; y < iDim; y++)
	{
		for (int x = iDim - 1; x >= 0; x--)
		{
			if ('O' == map[x][y])
			{
				int iNewX = rollSouth(map, x, y, iDim);
				if (iNewX != x)
				{
					map[iNewX][y] = 'O';
					map[x][y] = '.';
				}
			}
		}
	}
}


int rollEast(char **map, int x, int y, int iDim)
{
	int iNewY = y + 1;

	while(iNewY < iDim)
	{
		if ('#' == map[x][iNewY] || 'O' == map[x][iNewY])
			break;

		iNewY++;
	}

	return iNewY - 1;
}

void tiltEast(char **map, int iDim)
{
	for (int x = 0; x < iDim; x++)
	{
		for (int y = iDim - 1; y >= 0; y--)
		{
			if ('O' == map[x][y])
			{
				int iNewY = rollEast(map, x, y, iDim);
				if (iNewY != y)
				{
					map[x][iNewY] = 'O';
					map[x][y] = '.';
				}
			}
		}
	}
}

int rollWest(char **map, int x, int y, int iDim)
{
	int iNewY = y - 1;

	while(iNewY >= 0)
	{
		if ('#' == map[x][iNewY] || 'O' == map[x][iNewY])
			break;

		iNewY--;
	}

	return iNewY + 1;
}

void tiltWest(char **map, int iDim)
{
	for (int x = 0; x < iDim; x++)
	{
		for (int y = 0; y < iDim; y++)
		{
			if ('O' == map[x][y])
			{
				int iNewY = rollWest(map, x, y, iDim);
				if (iNewY != y)
				{
					map[x][iNewY] = 'O';
					map[x][y] = '.';
				}
			}
		}
	}
}

void spin(char **map, int iDim)
{
	tiltNorth(map, iDim);
	tiltWest(map, iDim);
	tiltSouth(map, iDim);
	tiltEast(map, iDim);
}

/**
 * Returns 0 if the two maps are identical, 1 otherwise
*/
int compare(char **m1, char **m2, int iDim)
{
	for(int x = 0; x < iDim; x++)
	{
		for (int y = 0; y < iDim; y++)
		{
			if (m1[x][y] != m2[x][y])
				return 1;
		}
	}
	return 0;
}

/**
 * Given two equal maps that are in a loop, count the size of the loop
*/
int countCycles(char **m1, char **m2, int iDim)
{
	int spins = 1;
	spin(m2, iDim);
	while (compare(m1, m2, iDim))
	{
		spin(m2, iDim);
		spins++;
	}

	return spins;
}

/**
 * Returns the northern load for the given map.
*/
int countLoad(char **map, int iDim)
{
	int iLoad = 0;
	for (int x = 0; x < iDim; x++)
	{
		for (int y = 0; y < iDim; y++)
		{
			if ('O' == map[x][y])
			{
				iLoad += iDim - x;
			}
		}
	}
	return iLoad;
}

/**
 * You are given a description of a flat plane with square cubes '#' and rocks 'O':
 * 
 * O....#....
 * O.OO#....#
 * .....##...
 * OO.#O....O
 * .O.....O#.
 * O.#..O.#.#
 * 
 * The plane can be tilted north, south, east and west causing the rocks to roll.
 * After tilting to the north, the map looks like this:
 * 
 * OOOO.#.O..
 * OO..#....#
 * O...O##..O
 * O..#.O....
 * ........#.
 * ..#....#.#
 * 
 * Each rock contributes to the "load" on a given support. The load from a single rock
 * on the north support is the number of rows from the rock to the opposite support, 
 * including the row the rock is on.
 * "Spin" the plane by tilting it north, west, south, and east in that order.
 * After 1000000000 spin cycles, calculate the load on the northern supports.
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day14(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;

	int iDim = 0;
	char **map = (char **)malloc(sizeof(char *) * 102);
	for (int i = 0; i < 102; i++)
	{
		map[i] = (char *)malloc(sizeof(char) * 102);
	}

	char **map2 = (char **)malloc(sizeof(char *) * 102);
	for (int i = 0; i < 102; i++)
	{
		map2[i] = (char *)malloc(sizeof(char) * 102);
	}

	int iLoad = 0;
	int iCycleLen = 0;
	int iCountToCycle = 0;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		strcpy(map[iDim], pCur);
		strcpy(map2[iDim], pCur);
		iDim++;
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	//start the maps offset by one spin
	spin(map2, iDim);

	//Two pointer loop detection: each cycle, spin the first map once and
	//spin the second map twice. Eventually the maps will be identical given that there 
	//is a loop.
	for(long i = 0; i < 1000; i++)
	{
		spin(map, iDim);
		spin(map2, iDim);
		if (!compare(map, map2, iDim))
		{
			//Found a loop
			iCycleLen = countCycles(map, map2, iDim);
			iCountToCycle = i + 1;
			break; 
		}

		//second spin
		spin(map2, iDim);
		if (!compare(map, map2, iDim))
		{
			//Found a loop
			iCycleLen = countCycles(map, map2, iDim);
			iCountToCycle = i + 1;
			break;
		}
	}

	//after we have determined we are in the loops, calculate the remaining cycles
	long lRemainingCycles = 1000000000 - (long)iCountToCycle;
	lRemainingCycles = lRemainingCycles % (long)iCycleLen;

	//printf("cyclelen is %d, before cycle %d, remaining to spin is %ld\n", iCycleLen, iCountToCycle, lRemainingCycles);
	for (long i = 0; i < lRemainingCycles; i++)
	{
		spin(map, iDim);
	}
	iLoad = countLoad(map, iDim);
	printf("done, load is %d\n", iLoad);
}

int main(int argc, char **argv)
{
	char *szInputFile = NULL;
	FILE *pFile = NULL;
	int ret = 0;

	szInputFile = (char *)malloc(256 * sizeof(char));
	if (NULL == szInputFile)
	{
		printf("Failed to allocate memory\n");
		return -1;
	}

	strcpy(szInputFile, argc > 1 ? argv[1] : INPUTNAME);

	pFile = fopen(szInputFile, "r");
	if (NULL == pFile)
	{
		printf("Failed to open %s\n", szInputFile);
		return -1;
	}

	day14(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
