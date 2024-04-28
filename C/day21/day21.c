#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 200
#define NUM_STEPS 6
#define NUM_BIGSTEPS 1050
#define BIG_SCALE 17

static int dirs[4][2] = 
{
	{0, 1},
	{1, 0},
	{0, -1},
	{-1, 0}
};

typedef struct step
{
	int row;
	int col;
	struct step *next;
} STEP;

/// Linked list-based fifo queue 
typedef struct fifoqueue
{
	STEP *first;
	STEP *last;
} QUEUE;

void q_add(QUEUE *q, STEP *p)
{
	p->next = NULL;
	if (q->first == NULL)
	{
		q->first = p;
		q->last = p;
	}
	else
	{
		q->last->next = p;
		q->last = p;
	}
}

STEP *q_pop(QUEUE *q)
{
	STEP *ret = NULL;
	if (q->first != NULL)
	{
		ret = q->first;
		if (q->first->next == NULL)
			q->last = NULL;

		q->first = q->first->next;
	}
	return ret;
}

void clearQueuedSteps(int **queued, int iDim)
{
	for (int i = 0; i < iDim; i++)
	{
		memset(queued[i], 0, iDim * sizeof(int));
	}
}

/**
 * Take the specified number of steps, counting how many possible locations
 * are accessible at each step.
 * Each step, count which plots we could be on and queue everything surrounding
 * it for the next step.
*/
void takeSteps(char **map, int iDim, int sRow, int sCol, int iSteps)
{
	printf("\n");
	QUEUE *queue = (QUEUE *)calloc(1, sizeof(QUEUE));

	//map of previously queued steps to avoid duplicate steps on the same plot
	int **queuedsteps = (int **)malloc(iDim * sizeof(int *));
	for (int i = 0; i < iDim; i++)
		queuedsteps[i] = (int *)malloc(iDim * sizeof(int));
	
	//Start at (sRow, sCol)
	queuedsteps[sRow][sCol] = 1;
	STEP *start = (STEP *)malloc(sizeof(STEP));
	start->row = sRow;
	start->col = sCol;
	start->next = NULL;
	q_add(queue, start);

	int iPossibleStepsToProcess = 1;
	int step = 0;
	for (step = 0; step < iSteps; step++)
	{
		clearQueuedSteps(queuedsteps, iDim);
		int iQueued = 0;
		//only process the steps in the queue for this current step id
		for (int i = 0; i < iPossibleStepsToProcess; i++)
		{
			STEP *s = q_pop(queue);
			//Add in each cardinal direction
			for (int dir = 0; dir < 4; dir++)
			{
				int nrow = s->row + dirs[dir][0];
				int ncol = s->col + dirs[dir][1];

				//Don't step on rocks and don't requeue steps
				if (nrow >= 0 && nrow < iDim &&
					ncol >= 0 && ncol < iDim &&
					map[nrow][ncol] != '#'   &&
					!queuedsteps[nrow][ncol])
				{
					queuedsteps[nrow][ncol] = 1;
					STEP *next = (STEP *)malloc(sizeof(STEP));
					next->row = nrow;
					next->col = ncol;
					next->next = NULL;
					q_add(queue, next);
					//printf("Step %d: Queued step (%d, %d)\n", step + 1, nrow, ncol);
					iQueued++;
				}
			}
			free(s);
		}
		iPossibleStepsToProcess = iQueued;

		//for part 2:
		if ((step+1) % 131 == 65)
			printf("%d: %d\n", step + 1, iPossibleStepsToProcess);
	}

	printf("\nDONE: %d possible locations after %d steps\n", iPossibleStepsToProcess, step);
}

//Print to console;
printBigMap(char **map, int iDim)
{
	for (int r = 0; r < iDim; r++)
	{
		for (int c = 0; c < iDim; c++)
		{
			printf("%c", map[r][c]);
		}
		printf("\n");
	}
}

/**
 * Given a map of a garden: 
 * '.' is empty ground
 * '#' is a rock
 * 'S' is the starting position
 * 
 * Find the number of possible locations he could be at in a certain number of steps
 * Part 1: 64
 * Part 2: 26501365
 * 
 * The entire map repeats indefinitely in all directions
 * 
 * Note: The part two goal distance is 26501365, which is 65 + 131 * 202300,
 *       65 being the number of steps to reach the edge of the map and 131 being
 *       the full square dimension of the map; The traversal forms a sequence in 
 *       that way:
 * 
 * 65: 3944 possible steps
 * 196: 35082
 * 327: 97230
 * 458: 190388
 * 589: 314556
 * 720: 469734
 * 851: 655922
 * 982: 873120
 * 
 * So step 26501365 is 634549784009844 .... thanks Wolfram
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day21(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;

	//size of the map: iDim x iDim
	int iDim = 0;
	char **map = NULL;
	char **bigmap = NULL;
	
	//current parsing index 
	int iCur = 0;

	//coordinates of the starting point in the map
	int sy = 0;
	int sx = 0;
	int bsy;
	int bsx;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		if (iDim == 0)
		{
			iDim = strlen(pCur) - 1;
			map = (char **)malloc((iDim) * sizeof(char *));
			bigmap = (char **)malloc((iDim * BIG_SCALE) * sizeof(char *));
			for (int i = 0; i < iDim * BIG_SCALE; i++)
			{
				bigmap[i] = (char *)malloc(((iDim * BIG_SCALE) + 2) * sizeof(char));
			}
		}

		//iDim + 1 to handle the '\n'
		map[iCur] = (char *)malloc((iDim + 1) * sizeof(char));
		strcpy(map[iCur], pCur);

		for (int row = 0; row < BIG_SCALE; row++)
		{
			for (int col = 0; col < BIG_SCALE; col++)
			{
				strcpy(&(bigmap[(iDim * row) + iCur][iDim * col]), pCur);
			}
		}

		//check for the start position
		char *s = strrchr(pCur, 'S');
		if (s != NULL)
		{
			sy = iCur;
			sx = s - pCur;

			printf("sy %d sx %d\n", sy, sx);
		}

		iCur++;
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	//duplicate the megamap for part 2
	for (int row = 0; row < iDim * BIG_SCALE; row++)
	{
		for (int col = 0; col < iDim * BIG_SCALE; col++)
		{
			if (bigmap[row][col] == 'S')
			{
				//remove the duplicate S
				if (row != (iDim * BIG_SCALE) / 2 ||
					col != (iDim * BIG_SCALE) / 2)
					bigmap[row][col] = '.';
				else
				{
					bsy = row;
					bsx = col;
				}
			}
		}
	}

	//printBigMap(bigmap, iDim * BIG_SCALE);

	//part 1
	//takeSteps(map, iDim, sy, sx, NUM_STEPS);

	//part 2
	printf("iDim %d, bsy %d bsx %d\n", iDim * BIG_SCALE, bsy, bsx);
	takeSteps(bigmap, iDim * BIG_SCALE, bsy, bsx, NUM_BIGSTEPS);
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

	day21(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
