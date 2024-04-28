#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 150
#define PART 2
#define INTERROW 123
#define INTERCOL 127

typedef enum direction
{
	RIGHT,
	DOWN,
	LEFT,
	UP
} DIR;
static int dirs[4][2] = 
{
	{0, 1},
	{1, 0},
	{0, -1},
	{-1, 0}
};

//Paths 
typedef struct path_list
{
	int row;
	int col;
} PATH;

/**
 * Recursively walk the path, taking all possible branches until we've found the 
 * longest path that actually reaches the end
*/
int traverse(char **map, int **visited, int iDim, PATH *current, int iLen, int iTargetRow, int iTargetCol)
{
	int iMaxLen = 0;
	PATH nextSteps[4];
	int iSteps = 0;

	//This is a valid path only if we reach the target
	if (current->col == iTargetCol && current->row == iTargetRow)
	{
		visited[current->row][current->col] = 0;
		//printf("Reached end in %d steps!!!\n", iLen);
		return iLen;
	}

	//printf("Traversing, current is (%d, %d): %c, iLen is %d\n", current->row, current->col, map[current->row][current->col], iLen);

	//Mark this tile as visited to avoid repeating steps in the current path
	visited[current->row][current->col] = 1;

	switch (map[current->row][current->col])
	{
		//For Part 2, all slopes are treated as normal paths, so all 
		//cases will fall through to '.' below
		case '>':
#if PART != 2
			nextSteps[0].row = current->row + dirs[RIGHT][0];
			nextSteps[0].col = current->col + dirs[RIGHT][1];
			iSteps = 1;
			break;
#endif
		case 'v':
#if PART != 2
			nextSteps[0].row = current->row + dirs[DOWN][0];
			nextSteps[0].col = current->col + dirs[DOWN][1];
			iSteps = 1;
			break;
#endif
		case '.':
		
#if PART == 2
			//Optimization: The final intersection MUST be taken
			//in the direction of the destination tile or else the resulting 
			//path will be invalid.
			if (current->row == INTERROW && current->col == INTERCOL)
			{
				nextSteps[0].row = current->row + dirs[DOWN][0];
				nextSteps[0].col = current->col + dirs[DOWN][1];
				iSteps = 1;
				break;
			}
#endif

			//Otherwise queue all cardinal directions
			for (int i = 0; i < 4; i++)
			{
				nextSteps[i].row = current->row + dirs[i][0];
				nextSteps[i].col = current->col + dirs[i][1];
			}
			iSteps = 4;
			break;
		default:
			printf("UNEXPECTED TILE %c\n", map[current->row][current->col]);
			return 0;
			break;
	}

	//Get next steps
	for (int i = 0; i < iSteps; i++)
	{
		int nRow = nextSteps[i].row;
		int nCol = nextSteps[i].col;

		//Can't walk on trees and previously-visited tiles
		if (map[nRow][nCol] != '#' &&
			!visited[nRow][nCol])
		{
			int iCurLen = traverse(map, visited, iDim, &(nextSteps[i]), iLen + 1, iTargetRow, iTargetCol);
			if (iCurLen > iMaxLen)
				iMaxLen = iCurLen;
		}
	}

	//Wipe our memory of steps as we bubble back up the call stack
	visited[current->row][current->col] = 0;
	return iMaxLen;
}

//Print to console
void printMap(char **map, int iDim)
{
	for (int row = 0; row < iDim; row++)
	{
		for (int col = 0; col < iDim; col++)
		{
			printf("%c", map[row][col]);
		}
		printf("\n");
	}
	printf("\n");
}

/**
 * Given a map of a forest:
 * '.' is open walking path
 * '#' is a tree that you can't walk on
 * '>' and 'v' are slopes, requiring you to go in that direction for one step
 * 
 * Find the LONGEST path that you can use to walk from the top left to the bottom
 * right of the map without stepping on a cell more than once.
 * 
 * 
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day23(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;

	int iDim = 0;
	int iCur = 0;
	char **map;

	int **visitedmap;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		if (iDim == 0)
		{
			iDim = strlen(pCur) - 1;
			map = (char **)malloc((iDim) * sizeof(char *));
			visitedmap = (int **)malloc((iDim) * sizeof(int *));
		}

		//iDim + 1 to handle the '\n'
		map[iCur] = (char *)malloc((iDim + 1) * sizeof(char));
		visitedmap[iCur] = (int *)malloc((iDim + 1) * sizeof(int));
		strcpy(map[iCur], pCur);

		iCur++;
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	PATH start;
	start.row = 1;
	start.col = 1;
	visitedmap[1][1] = 1;

	int longest = traverse(map, visitedmap, iDim, &start, 1, iDim - 1, iDim - 2);
	printf("longest is %d\n", longest);
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

	day23(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
