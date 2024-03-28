#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 150
#define GALAXY_DIM 200
#define EXPANSION_SIZE 1000000

typedef struct galaxy
{
	int index;
	int x;
	int y;
} GALAXY;

typedef struct edge
{
	int bVisited;
	int distance;
} EDGE;

/**
 * "Vertical" implementation of strchr, checking an entire column in the given
 * 2-D char array for a specific character.
 * 
 * in: strmap - 2-D char array
 * in: y - the index of the column to search
 * in: iNumX - the number of rows in the array
 * in: c - the character to search for
 * return: pointer to the first occurrence of the character, or NULL if not found
*/
char *vertstrchr(char **strmap, int y, int iNumX, char c)
{
	char *ret = NULL;

	for (int x = 0; x < iNumX; x++)
	{	
		if (strmap[x][y] == c)
		{
			ret = &(strmap[x][y]);
			break;
		}
	}

	return ret;
}

/**
 * Searches the given map for expansion points. If a given row or column is 
 * filled with empty space and no galaxies, it is considered expanded. The 
 * supplied expansion indicator arrays(bxExp and byExp, allocated by the caller)
 * are marked for each row and column that needs expansion
*/
void findExpansions(char **strmap, int iMapX, int iMapY, int *bxExp, int *byExp)
{
	for(int x = 0; x < iMapX; x++)
	{
		//if the row is empty, expand
		if (NULL == strchr(strmap[x], '#'))
			bxExp[x] = 1;
	}
	for (int y = 0; y < iMapY; y++)
	{
		//if the column is empty, expand
		if (NULL == vertstrchr(strmap, y, iMapX, '#'))
			byExp[y] = 1;
	}
}

/**
 * Calculates the shortest distance between two galaxies in the grid.
 * Since you can only travel in the four cardinal directions, going diagonally
 * is not shorter than simply calculating the 90-degree path:
 * *****
 * *#***
 * *|***
 * *L-#*
 * 
 * Each expanded row or column adds its expansion factor to the distance,
 * so if each expanded area is doubled, the distance in increased by one for
 * each expansion on the path.
*/
long calcDistance(GALAXY *g1, GALAXY *g2, int *bxExp, int *byExp)
{
	long lDist = 0;
	long lExpansions = 0;
	int xmax = g1->x >= g2->x ? g1->x : g2->x;
	int xmin = g1->x < g2->x ? g1->x : g2->x;
	int ymax = g1->y >= g2->y ? g1->y : g2->y;
	int ymin = g1->y < g2->y ? g1->y : g2->y;

	if (xmax != xmin)
	{
		for(int i = xmin; i <= xmax; i++)
		{
			if (bxExp[i])
				lExpansions++;
		}
	}

	if (ymax != ymin)
	{
		for(int i = ymin; i <= ymax; i++)
		{
			if (byExp[i])
				lExpansions++;
		}
	}

	return (lExpansions * (EXPANSION_SIZE - 1)) + xmax - xmin + ymax - ymin;
}

/**
 * Given a map of space(.) and galaxies(#), find the sum of the distance between
 * each pair of galaxies when traveling in the four cardinal directions.
 * 
 * ...#.
 * .....
 * #....
 * 
 * Each row or column of completely empty space is "expanded" by a given factor:
 * x2 in part one and x1000000 in part 2.
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day11(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;
	int iGalaxySize = 20;
	int iNumG = 0;
	int iRow = 0;

	int iMapX = 0;
	int iMapY = 0;

	long lSumDistances = 0;

	int *bxExpansions = (int *)calloc(GALAXY_DIM, sizeof(int));
	int *byExpansions = (int *)calloc(GALAXY_DIM, sizeof(int));
	GALAXY **galaxies = (GALAXY **)malloc(iGalaxySize * sizeof(GALAXY *));

	//
	char **strmap = (char **)malloc(GALAXY_DIM * sizeof(char *));
	for(int i = 0; i < GALAXY_DIM; i++)
	{
		strmap[i] = (char *)calloc(GALAXY_DIM, sizeof(char));
	}

	//parse file lines into the map
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		strcpy(strmap[iMapX], pCur);
		iMapX++;
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	iMapY = strlen(strmap[0]) - 1;

	//Calculate all expansions
	findExpansions(strmap, iMapX, iMapY, bxExpansions, byExpansions);

	//For each galaxy in the map, add a record of the galaxy to the galaxy list
	//and calculate its distance to each previously found galaxy
	for (int x = 0; x < iMapX; x++)
	{
		for (int y = 0; y < iMapY; y++)
		{
			if (strmap[x][y] == '#')
			{
				if (iNumG == iGalaxySize)
				{
					iGalaxySize *= 2;
					galaxies = (GALAXY **)realloc(galaxies, iGalaxySize * sizeof(GALAXY *));
				}

				galaxies[iNumG] = (GALAXY *)malloc(sizeof(GALAXY));
				galaxies[iNumG]->index = iNumG;
				galaxies[iNumG]->x = x;
				galaxies[iNumG]->y = y;

				for (int g = 0; g < iNumG; g++)
				{
					lSumDistances += calcDistance(galaxies[g], galaxies[iNumG], bxExpansions, byExpansions);
					//printf("Current sum is %ld\n", lSumDistances);
				}

				iNumG++;
			}
		}
	}

	printf("Sum of distances is %ld\n", lSumDistances);

	for (int i = 0; i < iNumG; i++)
		free(galaxies[i]);
	free(galaxies);
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

	day11(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
