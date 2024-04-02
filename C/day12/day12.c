#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include "simplehash.h"

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 50

/**
 * Recursively finds the number of valid configurations of damaged spring groups
 * in the given sequence. Memoization is used to avoid recomputation of long sequences.
 * 
 * Each possible situation is defined by 4 unique variables used as a key tuple for the cache:
 * - The spring sequence
 * - The index of the current broken spring group we are testing
 * - Whether or not a group is currently being parsed
 * - The number of damaged springs that we've accounted for in the group
 * 
 * Inputs:
 * sequence - The null-terminated sequence : for ex. "???.###"
 * groups   - The list of damaged spring group lengths
 * iGroups  - The size of the groups list
 * iGind    - The group index we are currently checking
 * bInGroup - Flag indicated that a group is currently being parsed/evaluated
 * iGrpL    - The number of springs accounted for in the current group
 * table    - Hash table storing the cache of previously calculated sequences 
 * 
 * Return: The number of valid configurations in the sequence
*/
unsigned long findValidConfigs(char *sequence, short *groups, short iGroups, short iGind, short bInGroup, short iGrpL, SEQUENCE **table)
{
	SEQUENCE *found;
	
	unsigned long lValid = 0;
	short iGroupIndex = iGind;
	short iGroupLen = iGrpL;

	//Check the cache to see if we've checked this exact sequence combo before
	if (NULL != (found = ht_get(table, sequence, iGroupIndex, bInGroup, iGroupLen)))
		return found->lValid;

	if (*sequence == '.')
	{
		//If we were parsing a group, that length should match the current expected descriptor
		if (bInGroup)
		{
			if (iGroupLen != groups[iGroupIndex])
			{
				lValid = 0;
				goto done;
			}

			iGroupIndex++;
		}
		lValid = findValidConfigs(sequence + 1, groups, iGroups, iGroupIndex, 0, 0, table);
		goto done;
	}

	if (*sequence == '?')
	{
		//Branch here, testing both possibilities for the wildcard
		unsigned long configs = 0;
		*sequence = '.';
		configs += findValidConfigs(sequence, groups, iGroups, iGroupIndex, bInGroup, iGroupLen, table);
		*sequence = '#';
		configs += findValidConfigs(sequence, groups, iGroups, iGroupIndex, bInGroup, iGroupLen, table);
		*sequence = '?';
		lValid = configs;
		goto done;
	}

	if (*sequence == '#')
	{
		if (bInGroup)
		{
			//If the current group is larger than the current expected size, this is invalid
			if (groups[iGroupIndex] <= iGroupLen)
			{
				lValid = 0;
				goto done;
			}
			lValid = findValidConfigs(sequence + 1, groups, iGroups, iGroupIndex, 1, iGroupLen + 1, table);
			goto done;
		}
		else //we are starting a new sequence
		{
			//If we've already covered every damage group, reject the sequence
			if (iGroupIndex == iGroups)
			{
				//printf("alread hit %d groups!!! %s\n", iGroups, sequence);
				lValid = 0;
				goto done;
			}

			//on to the next character
			lValid = findValidConfigs(sequence + 1, groups, iGroups, iGroupIndex, 1, iGroupLen + 1, table);
			goto done;
		}
	}

	//If we're here, we're done. See if we've processed all groups
	if (bInGroup)
	{
		//If the end of the sequence is a group of damaged springs, the current group
		//length must match the final group descriptor
		if (iGroupLen != groups[iGroupIndex])
		{
			lValid = 0;
			goto done;
		}
		iGroupIndex++;
	}

	if (iGroupIndex == iGroups)
	{
		lValid = 1;
		goto done;
	}

	//We haven't covered every group, reject
	lValid = 0;
	goto done;

done:
	//save the result in the cache
	ht_add(table, sequence, iGind, bInGroup, iGrpL, lValid);
	return lValid;
}

/**
 * Given a list of damage records, find the sum of all possible configurations
 * of damaged springs.
 * 
 * For example:
 * .??..??...?##. 1,1,3
 * '.' represents a functional spring
 * '#' represents a damaged spring
 * '?' is unknown, it could be either functional or damaged
 * 
 * The list of numbers represents the size of each contiguous group of damaged
 * springs in order from left to right. Given the wildcards(?) there are 4 possible
 * configurations in the sequence above.
 * 
 * For part two, each given record and grouping list is repeated four times
 * The record is replaced by 5 copies of itself separated by a '?'
 * 
 * So .# 1
 * becomes .#?.#?.#?.#?.# 1,1,1,1,1
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day12(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;

	char *tokptr = NULL;

	short groups[100];
	short iGroups = 0;
	unsigned long lValidConfigs = 0;

	SEQUENCE **table = ht_create();

	char seq[150] = {0};

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		//printf("Finding valid configs of %s\n", pCur);
		iGroups = 0;
		char *sequence = strtok(pCur, " ");
		strcpy(seq, sequence);
		char *szgroupnum;
		while (NULL != (szgroupnum = strtok(NULL, ",")))
		{
			groups[iGroups++] = atoi(szgroupnum);
		}

		//Sequence multiplication for part 2
		//Each row and spring groupings have four copies added before calculation
		//Each row has an extra '?' added between each copy 
		{
			for (int i = 0; i < 4; i++)
			{
				int len = strlen(seq);
				seq[len] = '?';
				
				strcpy(seq + len + 1, sequence);
				seq[strlen(seq)] = '\0'; 

				for (int j = 0; j < iGroups; j++)
				{
					groups[((i + 1) * iGroups) + j] = groups[j]; 
				}
			}
			iGroups *= 5;
		}

		unsigned long lConfigs = findValidConfigs(seq, groups, iGroups, 0, 0, 0, table);
		lValidConfigs += lConfigs;

		//Cached results for previous sequences do not matter, so wipe the table each time
		ht_clear(table);
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	printf("%lu total valid configs\n", lValidConfigs);
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

	day12(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
