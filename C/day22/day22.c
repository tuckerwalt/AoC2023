#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 50
#define MAX_BRICKS 1500
#define XMAX 10
#define YMAX 10
#define ZMAX 360

typedef enum blockaxis
{
	BLOCK,
	X,
	Y,
	Z
} TYPE;

typedef struct brick
{
	int x1;
	int y1;
	int z1;
	int x2;
	int y2;
	int z2;

	int len;
	TYPE type;

	struct brick *supportingBricks[5];
	int iSupporting;
	struct brick *bricksAbove[5];
	int iAbove;

	int bFalling;
} BRICK;

static int compareHeights(const void *brick1, const void *brick2)
{
	return (((BRICK *)brick1)->z1 - ((BRICK *)brick2)->z1);
}

//Compares bricks, counting higher bricks as less than smaller.
//Inputs are pointers to the indexes of the bricks in the vBricks array
static int compareHeightsReverse(const void *bInd1, const void *bInd2, void *vBricks)
{
	BRICK *bricks = vBricks;
	int *b1 = (int *)bInd1;
	int *b2 = (int *)bInd2;
	BRICK *brick1 = &(bricks[*b1]);
	BRICK *brick2 = &(bricks[*b2]); 

	return (brick2->z1 - brick1->z1);
}

//Fill the given pointer array with pointers to bricks below the given brick
void getBelow(BRICK ****map, BRICK *b, BRICK **below)
{
	int iCount = 0;

	switch (b->type)
	{
		case X:
			for (int x = b->x1; x <= b->x2; x++)
			{
				below[iCount++] = map[x][b->y1][b->z1 - 1];
			}
			break;
		case Y:
			for (int y = b->y1; y <= b->y2; y++)
			{
				below[iCount++] = map[b->x1][y][b->z1 - 1];
			}
			break;
		case Z:
		case BLOCK:
			below[0] = map[b->x1][b->y1][b->z1 - 1];
			break;
	}
}

/**
 * Add each brick to its respective supporting/above lists
*/
void addSupportBlock(BRICK *falling, BRICK *support)
{
	int bFound = 0;
	for (int i = 0; i < falling->iSupporting; i++)
	{
		if (falling->supportingBricks[i] == support)
		{
			bFound = 1;
			break;
		}
	}
	if (!bFound)
	{
		falling->supportingBricks[falling->iSupporting] = support;
		falling->iSupporting += 1;
	}

	bFound = 0;
	for (int i = 0; i < support->iAbove; i++)
	{
		if (support->bricksAbove[i] == falling)
		{
			bFound = 1;
			break;
		}
	}
	if (!bFound)
	{
		support->bricksAbove[support->iAbove] = falling;
		support->iAbove += 1;
	}
}

/**
 * Place pointers to this brick in the map at its location
*/
void placeBrickInMap(BRICK ****map, BRICK *b)
{
	printf("Placing brick at rest: (%d,%d,%d)->(%d,%d,%d), type %d\n", b->x1, b->y1, b->z1, b->x2, b->y2, b->z2, b->type);
	switch (b->type)
	{
		case X:
			for (int x = b->x1; x <= b->x2; x++)
			{
				map[x][b->y1][b->z1] = b;
			}
			break;
		case Y:
			for (int y = b->y1; y <= b->y2; y++)
			{
				map[b->x1][y][b->z1] = b;
			}
			break;
		case Z:
			for (int z = b->z1; z <= b->z2; z++)
			{
				map[b->x1][b->y1][z] = b;
			}
			break;
		case BLOCK:
			map[b->x1][b->y1][b->z1] = b;
			break;
	}
}

/**
 * Evaluating from the ground up, make each brick fall until it touches something
 * (brick or ground) and when its resting add it to the 3-d map of bricks
*/
void applyGravity(BRICK ****map, int xMax, int yMax, int zMax, int iMaxLen, BRICK *bricks, int iBricks)
{
	
	//Lower each brick down to the ground
	for (int i = 0; i < iBricks; i++)
	{
		BRICK *b = &(bricks[i]);
		BRICK *below[iMaxLen];
		memset(below, 0, sizeof(BRICK *) * iMaxLen);
		

		while (b->z1 > 1)
		{
			getBelow(map, b, below);
			int bResting = 0;
			for (int bInd = 0; bInd < b->len; bInd++)
			{
				if (NULL != below[bInd])
				{
					bResting = 1;
					addSupportBlock(b, below[bInd]);
				}
			}
			
			if (bResting)
			{
				break;
			}

			//fall one unit
			b->z1 -= 1;
			b->z2 -= 1;
			
		}
		placeBrickInMap(map, b);
	}
}

/**
 * Cound the number of bricks that could be removed without any other bricks falling
 * as a result.
*/
int findNonSupportingBricks(BRICK *bricks, int iBricks)
{
	int iTot = 0;
	
	for (int i = 0; i < iBricks; i++)
	{
		BRICK *b = &(bricks[i]);

		if (b->iAbove)
		{
			for (int a = 0; a < b->iAbove; a++)
			{
				//If the brick above has multiple supporters we're safe to remove this
				if (b->bricksAbove[a]->iSupporting == 1)
					goto cont;
				if (b->bricksAbove[a]->iSupporting == 0)
				{
					printf("UNEXPECTED!!\n");
				}
			}
			iTot++;
		}
		else
		{
			iTot++;
		}
cont:
		continue;
		//printf("checking next\n");
	}

	return iTot;
}

/**
 * Count the number of bricks that would fall as a result of removing the
 * specified brick.
*/
int countBricksToFall(BRICK *bricks, int *brickIndexes, int ind)
{
	int iToFall = 0;
	BRICK *b = &(bricks[brickIndexes[ind]]);
	b->bFalling = 1;

	//Optimization: use fifo queue of blocks to process instead of scanning entire stack
	//Look at every brick higher in the air than this brick
	for (int i = ind - 1; i >= 0; i--)
	{
		b = &(bricks[brickIndexes[i]]);

		//For each brick, check if every brick that supports it is currently
		//falling in this chain reaction. If so, mark this brick as falling and
		//continue, adding it to the count.
		if (b->iSupporting)
		{
			b->bFalling = 1;
			for (int s = 0; s < b->iSupporting; s++)
			{
				if (!(b->supportingBricks[s]->bFalling))
				{
					b->bFalling = 0;
					break;
				}
			}
			if (b->bFalling)
			{
				iToFall++;
			}
		}
	}
	
	b = &(bricks[brickIndexes[ind]]);
	printf("(%d,%d,%d)->(%d,%d,%d) : %d to fall from removal\n", b->x1, b->y1, b->z1, b->x2, b->y2, b->z2, iToFall);
	return iToFall;
}

/**
 * Find the total number of bricks that could fall as a result of removing
 * each individual brick
*/
int findTotalBricksToFall(BRICK *bricks, int *brickIndexes, int iBricks)
{
	int iTot = 0;

	for (int i = 0; i < iBricks; i++)
	{
		BRICK *b = &(bricks[brickIndexes[i]]);

		//If nothing's above it, nothing will happen by removing it
		if (b->iAbove)
		{
			iTot += countBricksToFall(bricks, brickIndexes, i);

			//Reset our memory of which bricks were falling from the previous
			//chain reaction
			for (int cur = 0; cur < iBricks; cur++)
				bricks[cur].bFalling = 0;
		}
	}

	return iTot;
}

//Print to console
void printBricks(BRICK *bricks, int iBricks)
{
	for (int i = 0; i < iBricks; i++)
	{
		BRICK *b = &(bricks[i]);
		printf("(%d,%d,%d)->(%d,%d,%d) : %d\n", b->x1, b->y1, b->z1, b->x2, b->y2, b->z2, b->len);
	}
}

/**
 * Given a description of bricks:
 * 1,0,1~1,2,1
 * 0,0,2~2,0,2
 * 0,2,3~2,2,3
 * 0,0,4~0,2,4
 * 2,0,5~2,2,5
 * 0,1,6~2,1,6
 * 1,1,8~1,1,9
 * 
 * 
 * Each brick is described by its beginning and end block coordinates (x, y, z).
 * Each brick is a singgle straight line of blocks.
 * The input shows the bricks floating at a single point in time. A block at z=1 is 
 * resting on the ground.
 * 
 * Bricks never rotate and there is no complex physics, if any part of a brick 
 * is resting on any part of another brick, it is still and can support any other
 * bricks.
 * 
 * Let the bricks fall due to gravity until all are at rest. For part one, count
 * the number of bricks that could be removed without any other bricks falling
 * due to a chain reaction.
 * For part two, consider each individual brick, and count the number of bricks
 * that would start falling as a result of removing it. Sum all counts.
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day22(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;

	//array of all bricks
	BRICK *bricks = (BRICK *)malloc(MAX_BRICKS * sizeof(BRICK));
	int iBricks = 0;
	int maxlength = 0;

	//3-d map of the bricks. Each location in the map points to the brick 
	// containing it. A brick on the ground from (0,0,1) to (3,0,1) would have 
	//3 locations in the map all pointing to the same brick structure.
	BRICK ****map = (BRICK ****)malloc(XMAX * sizeof(BRICK ***));
	for (int x = 0; x < XMAX; x++)
	{
		map[x] = (BRICK ***)malloc(YMAX * sizeof(BRICK **));
		for (int y = 0; y < YMAX; y++)
		{
			map[x][y] = (BRICK **)calloc(ZMAX, sizeof(BRICK *));
		}
	}

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		BRICK *b = &(bricks[iBricks]);

		sscanf(pCur, "%d,%d,%d~%d,%d,%d", &b->x1, &b->y1, &b->z1, &b->x2, &b->y2, &b->z2);

		if (b->x1 != b->x2)
		{
			b->len = b->x2 - b->x1 + 1;
			b->type = X;
		}
		else if (b->y1 != b->y2)
		{
			b->len = b->y2 - b->y1 + 1;
			b->type = Y;
		}
		else if (b->z1 != b->z2)
		{
			b->len = b->z2 - b->z1 + 1;
			b->type = Z;
		}
		else
		{
			//printf("SINGLE BLOCK\n");
			b->len = 1;
			b->type = BLOCK;
		}

		printf("(%d,%d,%d)->(%d,%d,%d) : %d\n", b->x1, b->y1, b->z1, b->x2, b->y2, b->z2, b->len);
	
		if (b->len > maxlength)
		{
			maxlength = b->len; 
		}

		iBricks++;
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	//printf("max blocklen is %d\n", maxlength);

	//FIRST, sort the bricks by their height (ground up)
	qsort(bricks, iBricks, sizeof(BRICK), compareHeights);
	//printBricks(bricks, iBricks);

	//Start from the ground up, making all bricks fall until they rest on something
	applyGravity(map, XMAX, YMAX, ZMAX, maxlength, bricks, iBricks);
	//Count the bricks that are not the sole supporter
	int iFreeToRemove = findNonSupportingBricks(bricks, iBricks);
	printf("%d bricks can be removed\n", iFreeToRemove);

	//Now for part two, go from the top down counting how many bricks are sole-supported at each point

	//DON'T sort the brick array, each brick holds pointers to connected bricks 
	//and rearranging the brick array messes up the bricks

	//Instead, make an array of the indexes of each brick in the brick array and
	//sort that index array by brick height(top down)
	int *brickIndexes = (int *)malloc(iBricks * sizeof(int));
	for (int i = 0; i < iBricks; i++)
		brickIndexes[i] = i;
	qsort_r(brickIndexes, iBricks, sizeof(int), compareHeightsReverse, (void *)bricks);
	/*
	for (int i = 0; i < iBricks; i++)
	{
		BRICK *b = &(bricks[brickIndexes[i]]);
		printf("%d - (%d,%d,%d)->(%d,%d,%d) : %d\n", brickIndexes[i], b->x1, b->y1, b->z1, b->x2, b->y2, b->z2, b->len);
	}
	*/

	int iTotFalling = findTotalBricksToFall(bricks, brickIndexes, iBricks);
	printf("Part 2: total bricks that could fall from removals is %d\n", iTotFalling);
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

	day22(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
