#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 150

typedef enum direction
{
	UP,
	DOWN,
	LEFT,
	RIGHT
} DIR;

/*
| is a vertical pipe connecting north and south.
- is a horizontal pipe connecting east and west.
L is a 90-degree bend connecting north and east.
J is a 90-degree bend connecting north and west.
7 is a 90-degree bend connecting south and west.
F is a 90-degree bend connecting south and east.
. is ground; there is no pipe in this tile.
S is the starting pipe.
*/

/**
 * Recursively traverse the map cell by cell.
 * sy and sx indicate the y and x coordinates of the current cell to look at.
 * prev indicats the direction that the PREVIOUS cell is in, and len is the
 * length of the loop up to this point.
 * 
 * Every cell in the loop is replaced by a character identifying that it is 
 * part of the loop.
 * 
 * | -> &
 * - -> d
 * L -> l
 * J -> j
 * 7 -> s
 * F -> f
*/
int traverse(char **map, int sy, int sx, DIR prev, int len)
{
	char c = map[sy][sx];
	
	switch(c)
	{
		case '|':
			map[sy][sx] = '&';
			if (DOWN == prev)
				return traverse(map, sy - 1, sx, DOWN, len + 1);
			else if (UP == prev)
				return traverse(map, sy + 1, sx, UP, len + 1);
			else
				return 0;
			break;
		case '-':
			map[sy][sx] = 'd';
			if (LEFT == prev)
				return traverse(map, sy, sx + 1, LEFT, len + 1);
			else if (RIGHT == prev)
				return traverse(map, sy, sx - 1, RIGHT, len + 1);
			else
				return 0;
			break;
		case 'L':
			map[sy][sx] = 'l';
			if (UP == prev)
				return traverse(map, sy, sx + 1, LEFT, len + 1);
			else if (RIGHT == prev)
				return traverse(map, sy - 1, sx, DOWN, len + 1);
			else
				return 0;
			break;
		case 'J':
			map[sy][sx] = 'j';
			if (LEFT == prev)
				return traverse(map, sy - 1, sx, DOWN, len + 1);
			else if (UP == prev)
				return traverse(map, sy, sx - 1, RIGHT, len + 1);
			else
				return 0;
			break;
		case '7':
			map[sy][sx] = 's';
			if (LEFT == prev)
				return traverse(map, sy + 1, sx, UP, len + 1);
			else if (DOWN == prev)
				return traverse(map, sy, sx - 1, RIGHT, len + 1);
			else
				return 0;
			break;
		case 'F':
			map[sy][sx] = 'f';
			if (DOWN == prev)
				return traverse(map, sy, sx + 1, LEFT, len + 1);
			else if (RIGHT == prev)
				return traverse(map, sy + 1, sx, UP, len + 1);
			else
				return 0;
			break;
		case 'S':
			return len;
			break;
		default:
			printf("ERROR: hit empty cell!!\n");
			return 0;
			break;	
	}

	return 0;
}

/**
 * Count the number of cells contained within the loop in the given map
 * Parse the map one row at a time and for each row, keep track of whether or not
 * the current cell would be considered "in" the loop. When parsing horizontally,
 * encountering a piece of pipe that is part of the loop means that future
 * cells are considered contained, until you encounter another piece of the loop.
 * 
 * |****| <- 4 cells within the loop
 * 
 * There are some exceptions to this: two corner pipes that do not round off the segment.
 * This is from 'F' followed by 'J' and 'L' followed by '7' (potentially with some '-' 
 * in between)
*/
int findContainedCells(char **map, int iDim)
{
	int iContained = 0;
	int bInCycle = 0;
	int bPotentialFJBend = 0;
	int bPotentialL7Bend = 0;
	for (int i = 0; i < iDim; i++)
	{
		//Initialize at the beginning of each row
		bInCycle = 0;
		bPotentialFJBend = 0;
		bPotentialL7Bend = 0;

		char *loop = map[i];
		while(*loop != '\n')
		{
			switch(*loop++)
			{
				case 'f':
					bPotentialFJBend = 1;
					break;
				case 'j':
					bPotentialFJBend = 0;
					if (!bPotentialL7Bend)
						bInCycle = !bInCycle;
					else
						bPotentialL7Bend = 0;
					break;
				case 'l':
					bPotentialL7Bend = 1;
					break;
				case 's':
					bPotentialL7Bend = 0;
					if (!bPotentialFJBend)
						bInCycle = !bInCycle;
					else
						bPotentialFJBend = 0;
					break;
				case '&':
					bInCycle = !bInCycle;
					break;
				case 'd':
					break;
				case '.':
				default:
					if (bInCycle)
						iContained++;
					break;
			}
			//printf("In cycle: %s\n", bInCycle ? "YES" : "NO");
		}
		//printf("%s", map[i]);
	}

	return iContained;
}

/**
 * You are given a map describing a pipe network:
 * 
 * .....
 * .S-7.
 * .|.|.
 * .L-J.
 * .....
 * 
 * | is a vertical pipe connecting north and south.
 * - is a horizontal pipe connecting east and west.
 * L is a 90-degree bend connecting north and east.
 * J is a 90-degree bend connecting north and west.
 * 7 is a 90-degree bend connecting south and west.
 * F is a 90-degree bend connecting south and east.
 * . is ground; there is no pipe in this tile.
 * S is the starting pipe.
 * 
 * There is guaranteed to be a loop starting from 'S', but some pipes on the map
 * will not be part of the loop. The map above has a loop length of 8.
 * 
 * Find the length of the loop and the number of cells(ground or pipe) that are 
 * not part of the loop but are CONTAINED within the loop. For example:
 * 
 * .S-------7.
 * .|F-----7|.
 * .||.....||.
 * .||.....||.
 * .|L-7F--J|.
 * .|..||.|F|.
 * .L--JL---J.
 * 
 * Even though this loop wraps around the cluster of ten tiles of ground, it
 * doesn't actually contain them. Only the 5 tiles on the second to last row:
 * ".." and ".|F" are contained withing the loop. 
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day10(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;

	//size of the map: iDim x iDim
	int iDim = 0;
	char **map = NULL;
	
	//current parsing index 
	int iCur = 0;

	//coordinates of the starting point in the map
	int sy = 0;
	int sx = 0;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		if (iDim == 0)
		{
			iDim = strlen(pCur) - 1;
			map = (char **)malloc((iDim) * sizeof(char *));
		}

		//iDim + 1 to handle the '\n'
		map[iCur] = (char *)malloc((iDim + 1) * sizeof(char));
		strcpy(map[iCur], pCur);

		//check for the start position
		char *s = strrchr(pCur, 'S');
		if (s != NULL)
		{
			sy = iCur;
			sx = s - pCur;
		}

		iCur++;
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	printf("STARTING POINT IS (%d, %d), %c\n\n", sy, sx, map[sy][sx]);

	//Traverse the loop and find the length

	//real input.txt
	int iLen = traverse(map, sy - 1, sx, DOWN, 1);

	//test input 1/2/3/4
	//int iLen = traverse(map, sy + 1, sx, UP, 1);


	//Rather than detect what kind of pipe the start is, I hardcode for my inputs.
	//for input.txt, the starting cell is an 'L'
	map[sy][sx] = 'l';
	
	//for testinputs
	//map[sy][sx] = 'f';
	//map[sy][sx] = 's';

	int iContained = findContainedCells(map, iDim);
	printf("\n\nLength of loop is %d, contained cells is %d\n", iLen, iContained);

	for (int i = 0; i < iDim; i++)
	{
		free(map[i]);
	}
	free(map);
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

	day10(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
