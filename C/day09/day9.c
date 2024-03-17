#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 250

#define MAX_SEQUENCE 50
#define DAY_PART 2

//Determines the next number in a sequence
int findNextNum(int *sequence, int iSeqLen)
{
	int *aDiffs = (int *)malloc(sizeof(int) * iSeqLen - 1);
	int bZeroes = 0;

	for (int i = 0; i < iSeqLen - 1; i++)
	{
		aDiffs[i] = sequence[i + 1] - sequence[i];
		if (aDiffs[i] != 0)
			bZeroes = 0;
	}

	//If diff seq is all zeroes, return;
	if (bZeroes)
	{
		free(aDiffs);
		return sequence[iSeqLen - 1];
	}
	else
	{	
		int ret = sequence[iSeqLen - 1] + findNextNum(aDiffs, iSeqLen - 1);
		free(aDiffs);
		return ret;
	}
}

//Determines the number that would come before the first number in a sequence
int findPrevNum(int *sequence, int iSeqLen)
{
	int *aDiffs = (int *)malloc(sizeof(int) * iSeqLen - 1);
	int bZeroes = 1;

	for (int i = 0; i < iSeqLen - 1; i++)
	{
		aDiffs[i] = sequence[i + 1] - sequence[i];
		if (aDiffs[i] != 0)
			bZeroes = 0;
	}

	//If diff seq is all zeroes, return;
	if (bZeroes)
	{
		free(aDiffs);
		return sequence[0];
	}
	else
	{	
		int ret = sequence[0] - findPrevNum(aDiffs, iSeqLen - 1);
		free(aDiffs);
		return ret;
	}
}

/**
 * Given lists of numbers, find the next number in sequence.
 * Do this by recursively generating list of the differences of the numbers
 * in the sequence until the sequence does not change, and add the current difference
 * to the end of the sequence. Given the sequence:
 *  0 3 6 9 12 15
 * 
 * The resulting diffs and next numbers can be visualized as:
 * 
 * 0   3   6   9  12  15  18
 *   3   3   3   3   3   3
 *     0   0   0   0   0
 * 
 * Add the new numbers from each sequence together. 
 * For part two, find the number that would come before the first number in each
 * sequence instead.
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day9(FILE *pFile, int part)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;

	char *tokptr = NULL;
	char *numtok = NULL;

	int sequence[MAX_SEQUENCE] = {0};
	int iSeqLen = 0;

	int iTotNewNumbers = 0;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		iSeqLen = 0;
		numtok = strtok_r(pCur, " ", &tokptr);

		sequence[iSeqLen++] = atoi(numtok);
		while(NULL != (numtok = strtok_r(NULL, " ", &tokptr)))
		{
			sequence[iSeqLen++] = atoi(numtok);
		}

		if (1 == part)
			iTotNewNumbers += findNextNum(sequence, iSeqLen);
		else if (2 == part)
			iTotNewNumbers += findPrevNum(sequence, iSeqLen);
	}
	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	printf("New num total is %d\n", iTotNewNumbers);
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

	day9(pFile, DAY_PART);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
