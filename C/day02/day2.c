#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdbool.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 250
#define NUM_BLUE  14
#define NUM_RED   12
#define NUM_GREEN 13

/**
 * Returns true if the provided game draws are possible
 * The game is possible if the individual draws of each color are less than or
 * equal to NUM_BLUE, NUM_RED, and NUM_GREEN respectively.
 * 
 * Regardless of whether or not the game is valid, also find the minimum "power"
 * of the given game. The power is equal to the MINIMUM amount of blue, red, and 
 * green cubes that would allow the game to be valid, multiplied together.
 * 
 * in:  szGameDraws - The string of game draws
 * out: pRetPower   - The minimum power of the game.
*/
bool isValidGame(char **szGameDraws, long *plRetPower)
{
	char *szCurrentDraw = NULL;
	int iDrawNum = 0;
	char szColor[10] = { 0 };
	long iMinBlue = 0;
	long iMinGreen = 0;
	long iMinRed = 0;
	bool bValid = true;

	while (true)
	{
		szCurrentDraw = strtok_r(NULL, ",;\n", szGameDraws);
		if (NULL == szCurrentDraw)
			break;

		//printf("current draw: %s\n", szCurrentDraw);
		if (EOF == sscanf(szCurrentDraw, "%d %s", &iDrawNum, szColor))
		{
			printf("ERROR reading %s: %s\n", szCurrentDraw, strerror(errno));
			return errno;
		}

		switch (szColor[0])
		{
			case 'b':
				if (iDrawNum > NUM_BLUE)
				{
					//printf("Error: Drew %d blue cubes but only %d exist!\n", iDrawNum, NUM_BLUE);
					bValid = false;
				}
				if (iDrawNum > iMinBlue)
					iMinBlue = iDrawNum;
				break;
			case 'g':
				if (iDrawNum > NUM_GREEN)
				{
					//printf("Error: Drew %d green cubes but only %d exist!\n", iDrawNum, NUM_GREEN);
					bValid = false;
				}
				if (iDrawNum > iMinGreen)
					iMinGreen = iDrawNum;
				break;
			case 'r':
				if (iDrawNum > NUM_RED)
				{
					//printf("Error: Drew %d red cubes but only %d exist!\n", iDrawNum, NUM_RED);
					bValid = false;
				}
				if (iDrawNum > iMinRed)
					iMinRed = iDrawNum;
				break;
			default:
				printf("Error: %s is not one of red, green or blue!\n", szColor);
				return false;
				break;
		}
	}
	
	*plRetPower = iMinBlue * iMinGreen * iMinRed;
	return bValid;
}

/**
 * You have a bag with 12 red cubes, 13 green cubes, and 14 blue cubes.
 * In one "game", a random number of cubes and their colors are picked and 
 * revealed, then placed back into the bag before more potential draws/picks.
 * 
 * Input is a set of games, one per line. Draws are separated by semicolons.
 * Ex:
 * Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
 * Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
 * Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
 * 
 * Find the sum of game IDs that are POSSIBLE given the original bag.
 * Game 3 is not possible because the bag does not have at least 20 red cubes.
 * 
 * Part two: Find the sum of the minimum "powers" of each game.
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day2_1(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;
	int iGameID = 0;
	int iSumGameIDs = 0;
	char *tokptr = NULL;
	char *gametoken = NULL;
	long lGamePow = 0;
	long lTotalPow = 0;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		tokptr = NULL;
		gametoken = strtok_r(pCur, ":", &tokptr);
		if (NULL == gametoken)
		{
			printf("ERROR SCANNING %s\n", pCur);
			continue;
		}

		if (EOF == sscanf(szBuffer, "Game %d", &iGameID))
		{
			printf("ERROR reading %s: %s\n", szBuffer, strerror(errno));
			return;
		}

		if (isValidGame(&tokptr, &lGamePow))
			iSumGameIDs += iGameID;

		lTotalPow += lGamePow;
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	printf("Part one game ID total is %d\n", iSumGameIDs);
	printf("Part two minimum power total is %ld\n", lTotalPow);

	return;
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

	day2_1(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
