#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 50

#define PART 2

/**
 * "Vertical" implementation of strcmp, comparing two columns in the given
 * 2-D char array.
 * 
 * in: strmap - 2-D char array
 * in: i1 - index of the first column
 * in: i2 - index of the second column
 * in: iNumX - the number of rows in the array
 * 
 * return: The number of characters that differ; 0 meaning that the columns are identical
*/
int vertstrcmp(char **strmap, int i1, int i2, int iNumX)
{
	int diff = 0;
	for (int x = 0; x < iNumX; x++)
	{	
		if (strmap[x][i1] != strmap[x][i2])
		{
			diff++;
		}
	}

	return diff;
}

//strcmp, but the return value is the number of characters that differ
int mystrncmp(char *s1, char *s2, int n)
{
	int diff = 0;
	for (int i = 0; i < n; i++)
	{
		if (s1[i] != s2[i])
			diff++;
	}

	return diff;
}

/**
 * Validates a mirror position(reflecting over a row) by checking how many characters 
 * in the image reflection don't match.
 * 
 * A return value of 0 means that this is a valid mirror, while a return of 5
 * indicates that 5 positions in the whole reflection don't match up
*/
int validmirrorrow(char **map, int rows, int cols, int i1, int i2)
{
	int first, second;
	int diff = 0;
	for (first = i1 - 1, second = i2 + 1; first >= 0 && second < rows; first--, second++)
	{
		diff += mystrncmp(map[first], map[second], cols);
	}

	return diff;
}

/**
 * Validates a mirror position(reflecting over a column) by checking how many characters 
 * in the image reflection don't match.
 * 
 * A return value of 0 means that this is a valid mirror, while a return of 5
 * indicates that 5 positions in the whole reflection don't match up
*/
int validmirrorcol(char **map, int rows, int cols, int i1, int i2)
{
	int first, second;
	int diff = 0;
	for (first = i1 - 1, second = i2 + 1; first >= 0 && second < cols; first--, second++)
	{
		diff += vertstrcmp(map, first, second, rows);
	}
	
	return diff;
}

/**
 * Returns the "score" for a given map. After finding the reflection line:
 *  - If the line is vertical, returns the number of columns left of the line
 *  - If the line is horizontal, returns the number of rows above the line multiplied by 100
*/
int getReflectionNumber(char **map, int rows, int cols)
{
	int ref = 0;
	int i1 = 0;
	int i2 = 0;

	int expecteddiff = PART - 1;
	int diff = 0;

	//first check rows...
	for (int i = 1; i < rows; i++)
	{
		i1 = i2;
		i2 = i;

		/**
		 * Weird logic here. In part 1, we're looking for a perfect reflection line, so we're
		 * expecting all comparisions(mystrncmp and validmirror*) to have zero differences.
		 * 
		 * In part two we are looking for exactly one difference in the entire reflection.
		 * If the initial possible reflection point has no differences, then the rest of the 
		 * evaluations must find exactly one bit that needs swapping. If the initial row/column
		 * needs a single bit swap, then the future evaluations must have zero necessary swaps.
		*/
#if PART==1
		if (!mystrncmp(map[i1], map[i2], cols))
		{
#else
		diff = mystrncmp(map[i1], map[i2], cols);
		if (0 == diff || 1 == diff)
		{
			if (diff)
				expecteddiff--;
#endif
			//printf("possible mirror row %d %d\n", i1+1, i2+1);
			if (expecteddiff == validmirrorrow(map, rows, cols, i1, i2))
			{
				return 100 * (i1 + 1);
			}
		}
		expecteddiff = PART - 1;
	}

	i1 = 0;
	i2 = 0;
	//then columns
	for (int i = 1; i < cols; i++)
	{
		i1 = i2;
		i2 = i;

#if PART==1
		if (!vertstrcmp(map, i1, i2, rows))
		{
#else
		diff = vertstrcmp(map, i1, i2, rows);
		if (0 == diff || 1 == diff)
		{
			if (diff)
				expecteddiff--;
#endif
			//printf("possible mirror col %d %d\n", i1+1, i2+1);
			if (expecteddiff == validmirrorcol(map, rows, cols, i1, i2))
			{
				return i1 + 1;
			}
		}
		expecteddiff = PART - 1;
	}

	int *pt = NULL;
	int dead = *pt;
	return 0;
}

/**
 * Given a list of maps of dots and hashes, find the sum of the reflection numbers
 * .#....#
 * ###..##
 * ...##..
 * #.#..#.
 * 1234567
 * 
 * The map above can be reflected symmetrically over columns 4 and 5 (since column 1 doesn't 
 * have a reflected column in that case, it is ignored).
 * The reflection number for vertical reflection lines like the case above is the number
 * of columns left of the line. in the map above, that would be 4.
 * For horizontal lines the resulting number is 100 multiplied by the number of rows above
 * the reflection line.
 * 
 * Each given map in the input has a different reflection line that would work if exactly 
 * one bit in the map was swapped, either from '.' to '#' or vice versa. For part two,
 * find the reflection numbers for those lines in the input. 
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day13(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;

	char **map = (char **)malloc(20 * sizeof(char *));
	char iMapX = 0;
	char iMapY = 0;

	long lReflectionNum = 0;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		if (pCur[0] == '\n')
		{
			int count = getReflectionNumber(map, iMapX, iMapY);
			lReflectionNum += (long)count;

			for (int i = 0; i < iMapX; i++)
				free(map[i]);
			iMapY = 0;
			iMapX = 0;
			continue;
		}

		int len = strlen(pCur);
		map[iMapX] = (char *)malloc(len * sizeof(char) + 1);
		strcpy(map[iMapX], pCur);
		
		iMapY = len - 1;
		iMapX++;
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}
	
	//calculate the final map
	int count = getReflectionNumber(map, iMapX, iMapY);
	lReflectionNum += (long)count;

	printf("Reflection number is %ld\n", lReflectionNum);
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

	day13(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
