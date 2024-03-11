#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>

#include "day3helpers.h"

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 150

/**
 * Given an engine "schematic", return the sum of the part numbers
 * A part number is a number that is adjacent to a symbol (non '.')
 * 
 * 467..114..
 * ...*......
 * ..35..633.
 * ......#...
 * 617*......
 * .....+.58.
 * ..592.....
 * ......755.
 * ...$.*....
 * .664.598..
 * 
 * In the above schematic, 114 and 58 are NOT part numbers
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day1_1(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;
	char engine[ENGINE_DIMENSION + 2][ENGINE_DIMENSION + 2];
	int iPartSum = 0;

	memset(engine, '.', (ENGINE_DIMENSION + 2) * (ENGINE_DIMENSION + 2));

	int iCurLine = 1;
	//Read the entire schematic into a 2d array
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		memcpy(&engine[iCurLine][1], pCur, ENGINE_DIMENSION);
		iCurLine++;
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	bool bFoundNum = false;
	int iNum = 0;
	char szNum[5] = { 0 };
	int iNumLength = 0;
	bool bValidNum = false;
	for (int i = 1; i < ENGINE_DIMENSION + 1; i++)
	{	
		for (int j = 1; j < ENGINE_DIMENSION + 1; j++)
		{
			/**
			 * For each number found, add it to the current number string if necessary,
			 * and check for surrounding symbols
			*/
			char c = engine[i][j];
			
			if (isdigit(c))
			{
				//Add the digit to the current num
				szNum[iNumLength++] = c;

				if (!bValidNum)
				{
					//If we are at the start of a new potential part number, check the 5 
					//cells around its left side
					if (!bFoundNum)
						bValidNum = checkAdjacentFirst(engine, i, j);
					//Otherwise, only check above and below the current digit
					else
						bValidNum = checkAdjacent(engine, i, j);
				}
				bFoundNum = true;
			}
			else
			{
				//Finish up the current number if needed
				if (bFoundNum)
				{
					//check this cell as well as above and below it
					if (!bValidNum)
						bValidNum = checkAdjacentLast(engine, i, j);

					//If this is a part number, add it
					if (bValidNum)
					{
						int iNum = atoi(szNum);
						//printf("Adding %d to parts sum\n", iNum);
						iPartSum += iNum;
					}
					memset(szNum, 0, 5);
					bValidNum = false;
					iNumLength = 0;
					bFoundNum = false;
				}
			}
			
		}

		//After each row, close off a number if we're currently looking at one
		if (bFoundNum)
		{
			//If this is a part number, add it
			if (bValidNum)
			{
				int iNum = atoi(szNum);
				//printf("Adding %d to parts sum\n", iNum);
				iPartSum += iNum;
			}
			memset(szNum, 0, 5);
			bValidNum = false;
			iNumLength = 0;
			bFoundNum = false;
		}
	}

	printf("Part one: Sum of parts is %d\n", iPartSum);
}

/**
 * Given an engine "schematic", return the sum of the gear ratios
 * A gear is a symbol (non '.') that is adjacent to exactly two part numbers,
 * and its ratio is the product of the two part numbers
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day1_2(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;
	char engine[ENGINE_DIMENSION + 2][ENGINE_DIMENSION + 2];
	long iRatioSum = 0;

	memset(engine, '.', (ENGINE_DIMENSION + 2) * (ENGINE_DIMENSION + 2));

	int iCurLine = 1;
	//Read the entire schematic into a 2d array
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		memcpy(&engine[iCurLine][1], pCur, ENGINE_DIMENSION);
		iCurLine++;
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	for (int i = 1; i < ENGINE_DIMENSION + 1; i++)
	{	
		for (int j = 1; j < ENGINE_DIMENSION + 1; j++)
		{
			/**
			 * For each symbol, check if it is a gear
			*/
			char c = engine[i][j];
			
			int n1x, n1y;
			int n2x, n2y;
			if (isSymbol(c))
			{
				if (isGear(engine, i, j, &n1x, &n1y, &n2x, &n2y))
				{
					iRatioSum += getPartNum(engine, n1x, n1y) * getPartNum(engine, n2x, n2y);
				}
			}
		}
	}

	printf("Part two: Sum of ratios is %ld\n", iRatioSum);
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

	day1_1(pFile);
	rewind(pFile);
	day1_2(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
