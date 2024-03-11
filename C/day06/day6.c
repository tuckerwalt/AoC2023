#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 40

/**
 * Given a race time and current record, find the number of ways
 * the record can be beaten.
*/
long long runRace(long long time, long long record)
{
	long long lMinWinHold = 0;
	long long lMaxWinHold = time + 1;

	//'i' represents the amount of time holding the button
	for (long long i = 1; i < time; i++)
	{
		long long dist = i * (time - i);
		if (dist > record)
		{
			lMinWinHold = i;
			break;
		}
	}

	for (long long i = time - 1; i > 0; i--)
	{
		long long dist = i * (time - i);
		if (dist > record)
		{
			lMaxWinHold = i;
			break;
		}
	}

	/*printf("Race: Min beat %lld, Max beat %lld, %lld ways to win\n", 
		lMinWinHold, lMaxWinHold, lMaxWinHold - lMinWinHold + 1);
	*/

	return lMaxWinHold - lMinWinHold + 1;
}

/**
 * Given a set of boat races, find the product of the number of ways
 * each race record can be beaten.
 * 
 * To race, you charge your boat for some amount of milliseconds, and then
 * run for the remaining seconds. Each millisecond, the boat travels the 
 * distance equal to the amount of time the boat was charged for.
 * Example: In a 9ms race, if you charge for 2ms and release, the boat will
 * travel 2 units/ms for the remaining 7ms, for a total of 14 units.
 * 
 * Ex input:
 * Time:      7  15   30
 * Distance:  9  40  200
 * 
 * Race 1: 7 ms, record of 9 units, record can be beaten 4 ways by charging 
 * for 2, 3, 4, or 5 ms
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day6_1(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;
	char *curtok = NULL;
	char *tokptr = NULL;

	int aiTimes[10] = {0};
	int aiRecords[10] = {0};
	int iRaces = 0;
	int iCnt = 0;
	int iParseStage = 0;

	int iProdRecords = 1;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		tokptr = NULL;
		curtok = strtok_r(pCur, ":", &tokptr);
		while (NULL != (curtok = strtok_r(NULL, " ", &tokptr)))
		{
			if (!iParseStage)
			{
				aiTimes[iCnt++] = atoi(curtok);
				iRaces++;
			}
			else
				aiRecords[iCnt++] = atoi(curtok);
		}

		iParseStage++;
		iCnt = 0; 
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	for (iCnt = 0; iCnt < iRaces; iCnt++)
	{
		iProdRecords *= runRace((long long)(aiTimes[iCnt]), (long long)(aiRecords[iCnt]));
	}

	printf("Part one: product of race-beating methods is %d\n", iProdRecords);
}

/**
 * Treat the original input as a definition for one mega race:
 * 
 * Time:      7  15   30
 * Distance:  9  40  200
 * 
 * This is a race lasting 71530 ms that has a record of 940200.
 * It can be beaten in 71503 different ways.
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day6_2(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;
	char *curtok = NULL;
	char *tokptr = NULL;

	char szTimes[25] = {0};
	char szRecords[25] = {0};
	int iTimeLen = 0;
	int iRecordLen = 0;

	int iCnt = 0;
	int iParseStage = 0;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		for (int i = 0; i < strlen(pCur); i++)
		{
			if(isdigit(pCur[i]))
			{
				if (!iParseStage)
					szTimes[iTimeLen++] = pCur[i];
				else
					szRecords[iRecordLen++] = pCur[i];
			}
		}
		iParseStage++;
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	printf("Part two: %lld ways to beat the mega-race\n", runRace(atoll(szTimes), atoll(szRecords)));
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

	day6_1(pFile);
	rewind(pFile);
	day6_2(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
