#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 100

bool IsDigit(char *pszC, int *pNum);

/**
 * On each line, the first digit encountered and the last digit encountered 
 * combine to form a calibration value. Find the sum of all calibration values.
 * 
 * a1b2c3d4e5f -> 15
 * treb7uchet  -> 77
 * 
 * Each line is guaranteed to contain a digit
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
int day1_1(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;
	int d1, d2;
	int iTotalCalibration = 0;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{	
		//first digit
		while (true)
		{
			if (isdigit(*pCur))
			{
				d1 = *pCur - '0';
				break;
			}
			pCur++;
		}

		//second digit
		pCur = &szBuffer[strlen(szBuffer) - 1];
		while (true)
		{
			if (isdigit(*pCur))
			{
				d2 = *pCur - '0';
				break;
			}
			pCur--;
		}

		//Add calibration
		iTotalCalibration += d2 + (10 * d1);
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return errno;
	}

	printf("Part one total calibration is %d\n", iTotalCalibration);
	return 0;
}

/**
 * On each line, the first digit encountered and the last digit encountered 
 * combine to form a calibration value. Find the sum of all calibration values.
 * 
 * Digits can be represented as actual digits (5) OR the name of the digit (five)
 * 
 * 7pqrstsixteen   -> 76
 * abcone2threexyz -> 13
 * 
 * Each line is guaranteed to contain a digit
 * Spelled digits are lowercase
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
int day1_2(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;
	int d1, d2;
	int iTotalCalibration = 0;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{	
		//first digit
		while (1)
		{
			if (IsDigit(pCur, &d1))
				break;
			pCur++;
		}

		//second digit
		pCur = &szBuffer[strlen(szBuffer) - 1];
		while (1)
		{
			if (IsDigit(pCur, &d2))
				break;
			pCur--;
		}

		//Add calibration
		iTotalCalibration += d2 + (10 * d1);
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return errno;
	}

	printf("Part two total calibration is %d\n", iTotalCalibration);
	return 0;
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

//Helpers

//gross
/**
 * Returns true if the input string either:
 * 	starts with a digit (1-9) OR
 * 	starts with a spelled out digit (one, two, three, etc)
 * 
 * Does not look for "zero"
 * 
 * in:  pszC - the string to look at
 * out: pNum - the digit, if true is returned
*/
bool IsDigit(char *pszC, int *pNum)
{
	if (isdigit(*pszC))
	{
		*pNum = *pszC - '0';
		return true;
	}

	if (!strncmp("one", pszC, strlen("one")))
	{
		*pNum = 1;
		return true;
	}
	if (!strncmp("two", pszC, strlen("two")))
	{
		*pNum = 2;
		return true;
	}
	if (!strncmp("three", pszC, strlen("three")))
	{
		*pNum = 3;
		return true;
	}
	if (!strncmp("four", pszC, strlen("four")))
	{
		*pNum = 4;
		return true;
	}
	if (!strncmp("five", pszC, strlen("five")))
	{
		*pNum = 5;
		return true;
	}
	if (!strncmp("six", pszC, strlen("six")))
	{
		*pNum = 6;
		return true;
	}
	if (!strncmp("seven", pszC, strlen("seven")))
	{
		*pNum = 7;
		return true;
	}
	if (!strncmp("eight", pszC, strlen("eight")))
	{
		*pNum = 8;
		return true;
	}
	if (!strncmp("nine", pszC, strlen("nine")))
	{
		*pNum = 9;
		return true;
	}

	return false;
}
