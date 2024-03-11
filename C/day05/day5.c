#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 250
#define MAX_MAPS 50
#define MAX_SEEDS 25
#define NUM_TRANSLATIONS 7
#define PART_TO_RUN 2

typedef struct mapping
{
	long long lSrcSt;
	long long lDstSt;
	long long lLen;
} MAP;

typedef enum parsing_stage {
	SEEDS,
	SEEDTOSOIL,
	SOILTOFERT,
	FERTTOWATER,
	WATERTOLIGHT,
	LIGHTTOTEMP,
	TEMPTOHUMID,
	HUMIDTOLOC
} STAGE;

/**
 * Brute forcing part 2
 * Given a starting seed number and range amount, test every seed in the range and return the
 * lowest location needed.
 * TODO implement a better way of testing seeds
*/
long long getMinimumSeedLocForRange(long long lSeed, long long lRange, MAP **maps, int *mapamounts)
{
	long long lMinLoc = __LONG_LONG_MAX__;

	for (long long r = 0; r < lRange; r++)
	{
		long long lCurNum = lSeed + r;

		for(int i = 0; i < NUM_TRANSLATIONS; i++)
		{
			for (int j = 0; j < mapamounts[i]; j++)
			{
				MAP curMap = maps[i][j];
				long long lDiff = lCurNum - curMap.lSrcSt;
				
				if (lDiff >= 0 && lDiff < curMap.lLen)
				{
					//found it!
					lCurNum = curMap.lDstSt + lDiff;
					break;
				}
			}
		}
		if (lCurNum < lMinLoc)
			lMinLoc = lCurNum;
	}
	return lMinLoc;
}

/**
 * Traverse the maps and find a planting location for a given seed
 * 
 * in:  lSeed      - The seed number
 * in:  maps       - The collection of maps
 * in:  mapamounts - The amount of maps per stage
*/
long long getSeedLoc(long long lSeed, MAP **maps, int *mapamounts)
{
	long long lCurNum = lSeed;

	for(int i = 0; i < NUM_TRANSLATIONS; i++)
	{
		for (int j = 0; j < mapamounts[i]; j++)
		{
			MAP curMap = maps[i][j];
			long long lDiff = lCurNum - curMap.lSrcSt;
			
			if (lDiff > 0 && lDiff < curMap.lLen)
			{
				//found it!
				lCurNum = curMap.lDstSt + lDiff;
				break;
			}
		}
	}

	return lCurNum;
}

/**
 * Given an "almanac" containing mappings of seeds to soils, etc...
 * Find the lowest location that a seed requires
 * 
 * seeds: 79 14 55 13
 *
 * seed-to-soil map:
 * 50 98 2
 * 52 50 48
 * 
 * soil-to-fertilizer map:
 * 0 15 37
 * 37 52 2
 * 39 0 15
 * 
 * fertilizer-to-water map:
 * 49 53 8
 * 0 11 42
 * 42 0 7
 * 57 7 4
 * 
 * water-to-light map:
 * 88 18 7
 * 18 25 70
 * 
 * light-to-temperature map:
 * 45 77 23
 * 81 45 19
 * 68 64 13
 * 
 * temperature-to-humidity map:
 * 0 69 1
 * 1 0 69
 * 
 * humidity-to-location map:
 * 60 56 37
 * 56 93 4
 * 
 * The maps are given as: 
 * 	<destination start id> <source start id> <length>
 * 
 * So for the first seed-soil mapping: 50 98 2
 * 		seed 98 needs soil 50
 * 		seed 99 needs soil 51
 * 
 * No mapping means the dest and src id are equal.
 * In the example, seed 100 needs soil 100
 * 
 * in: pFile - An open file pointer, freed by the caller
 * in: part  - Which part to execute
*/
void day5(FILE *pFile, int part)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;
	char *token;

	long long seeds[MAX_SEEDS];
	long long seedranges[MAX_SEEDS];

	int iSeeds = 0;
	long long lMinLocation = __LONG_LONG_MAX__;

	MAP **maps = (MAP **)malloc(NUM_TRANSLATIONS * sizeof(MAP *));
	if (NULL == maps)
	{
		printf("Memory error?\n");
		return;
	}
	for (int i = 0; i < NUM_TRANSLATIONS; i++)
	{
		maps[i] = (MAP *)malloc(MAX_MAPS * sizeof(MAP));
		if (NULL == maps[i])
		{
			printf("Memory error?\n");
			return;
		}
	}

	int mapamounts[NUM_TRANSLATIONS] = { 0 };

	STAGE stage = SEEDS;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		switch (stage)
		{
			case SEEDS:
				if (pCur[0] == '\n')
				{
					//printf("Moving to next stage\n");
					stage++;
					break;
				}

				//parse all seeds then progress
				strtok(pCur, " ");
				while (NULL != (token = strtok(NULL, " ")))
				{
					if (1 == part)
						seeds[iSeeds++] = atoll(token);
					else if (2 == part)
					{
						seeds[iSeeds] = atoll(token);
						token = strtok(NULL, " ");
						seedranges[iSeeds++] = atoll(token);
					}
					else
						return;
				}
				break;
			case SEEDTOSOIL:
			case SOILTOFERT:
			case FERTTOWATER:
			case WATERTOLIGHT:
			case LIGHTTOTEMP:
			case TEMPTOHUMID:
			case HUMIDTOLOC:
				if (pCur[0] == '\n')
				{
					//printf("Moving to next stage\n");
					stage++;
					break;
				}
				if (!isdigit(pCur[0]))
					continue;
				
				if (EOF == sscanf(pCur, "%lld %lld %lld", 
					&(maps[stage-1][mapamounts[stage-1]].lDstSt),
					&(maps[stage-1][mapamounts[stage-1]].lSrcSt),
					&(maps[stage-1][mapamounts[stage-1]].lLen)))
				{
					printf("ERROR SCANNING %s\n", pCur);
					return;
				}

				mapamounts[stage-1]++;
				break;
			default:
				printf("ERROR: Unrecognized parsing stage %d\n", stage);
				break;
		}
	}
	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	//Now find the location of each seed
	for (int i = 0; i < iSeeds; i++)
	{
		long long lSeedLoc;

		//part 1, no ranges
		if (1 == part)
			lSeedLoc = getSeedLoc(seeds[i], maps, mapamounts);
		//part 2, test ranges of seeds
		else if (2 == part)
		{
			lSeedLoc = getMinimumSeedLocForRange(seeds[i], seedranges[i], maps, mapamounts);
			printf("Min loc for range %lld, %lld is %lld\n", seeds[i], seedranges[i], lSeedLoc);
		}
		else
			return;

		//printf("Seed %lld has location %lld\n", seeds[i], lSeedLoc);
		if (lSeedLoc < lMinLocation)
		{
			lMinLocation = lSeedLoc;
		}
	}

	printf("Part %d: Minimum location is %lld\n", part, lMinLocation);
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

	day5(pFile, PART_TO_RUN);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
