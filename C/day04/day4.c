#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdbool.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 150
#define NUMBER_OF_CARDS 202

/**
 * Returns the point value of a given scratchcard and the number of matches it had
 * 
 * in:  pszGameDraws - an active strtok_r saveptr
 * out: piMatches    - the number of matches on the card
*/
int scratchCard(char **pszGameDraws, int *piMatches)
{
	char *szCurrentNum = NULL;
	int score = 0;
	int iCurrentNum = 0;
	char winningNums[100] = { 0 };
	int iMatches = 0;

	while (true)
	{
		szCurrentNum = strtok_r(NULL, " ", pszGameDraws);
		if (NULL == szCurrentNum)
		{
			printf("ERROR parsing scorecard!\n");
			return 0;
		}

		if ('|' == szCurrentNum[0])
			break;

		winningNums[atoi(szCurrentNum)] = 1;
	}

	for (szCurrentNum = strtok_r(NULL, " ", pszGameDraws); NULL != szCurrentNum; szCurrentNum = strtok_r(NULL, " ", pszGameDraws))
	{
		iCurrentNum = atoi(szCurrentNum);
		if (winningNums[iCurrentNum])
		{
			if (!score)
				score = 1;
			else
				score = score << 1;

			iMatches++;
		}
	}

	if (piMatches != NULL)
		*piMatches = iMatches;
	return score;
}

/**
 * Given a list of scratchcards, return the sum of the point values of the cards
 * Cards are given as a list of winning numbers followed by the numbers on the actual card
 * The point value of a card is 2^(matched winning numbers - 1)
 * 
 * Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53
 * This card has 4 matching winning numbers and thus a point value of 8
 * 
 * in: fp - An open file pointer, freed by the caller
*/
void day4_1(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;
	char *tokptr = NULL;
	char *cardtoken = NULL;
	int iSumCardPoints = 0;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		tokptr = NULL;
		cardtoken = strtok_r(pCur, ":", &tokptr);
		if (NULL == cardtoken)
		{
			printf("ERROR SCANNING %s\n", pCur);
			continue;
		}

		iSumCardPoints += scratchCard(&tokptr, NULL);
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	printf("Part one, scratchcards are worth %d points\n", iSumCardPoints);
}

/**
 * Given a list of scratchcards, return the total number of cards in the pile
 * A scratch card gives you copies of the (matched winning numbers) cards below it in the list
 * 
 * Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53
 * This card has 4 matching winning numbers and thus gives a copy of card 2, 3, 4, and 5
 * Card 2 would then be evaluated two times, and so on
 * 
 * in: fp - An open file pointer, freed by the caller
*/
void day4_2(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;
	char *tokptr = NULL;
	char *cardtoken = NULL;
	int iCardID = 0;
	long iSumCards = 0;
	long aiNumberOfCards[NUMBER_OF_CARDS + 1];

	for (int i = 0; i < NUMBER_OF_CARDS + 1; i++)
		aiNumberOfCards[i] = 1;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		tokptr = NULL;
		cardtoken = strtok_r(pCur, ":", &tokptr);
		if (NULL == cardtoken)
		{
			printf("ERROR SCANNING %s\n", pCur);
			continue;
		}

		if (EOF == sscanf(cardtoken, "Card %d", &iCardID))
		{
			printf("ERROR reading %s: %s\n", cardtoken, strerror(errno));
			return;
		}

		int iCurMatches = 0;
		scratchCard(&tokptr, &iCurMatches);

		long iAmountOfCurCardID = aiNumberOfCards[iCardID];
		iSumCards += iAmountOfCurCardID;

		for (int i = iCardID + 1; i < iCardID + 1 + iCurMatches; i++)
		{
			aiNumberOfCards[i] += iAmountOfCurCardID;
		}
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	printf("Part two, %ld total scratchcards in pile\n", iSumCards);
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

	day4_1(pFile);
	rewind(pFile);
	day4_2(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
