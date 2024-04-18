#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include "simplehash.h"

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 50
#define MAX_RULES 5

typedef struct rule
{
	char partCat;
	char cmp;
	int val;
	char dest[7];
} RULE;

typedef struct part
{
	int x;
	int m;
	int a;
	int s;
} PART;

/**
 * Returns the total number of part configurations that are accepted by the set
 * of given rules for the part category ranges. Recursively breaks the ranges 
 * up when a rule splits a category range.
*/
long validateAllAccepted(ENTRY **ruletable, RULE *cRules, int iR, int minX, int maxX, int minM, int maxM, int minA, int maxA, int minS, int maxS)
{
	int cMinX = minX;
	int cMaxX = maxX;
	int cMinM = minM;
	int cMaxM = maxM;
	int cMinA = minA;
	int cMaxA = maxA;
	int cMinS = minS;
	int cMaxS = maxS;

	long lTotal = 0;
	int iRule = iR;
	RULE *curRules = cRules;

	//printf("Validating x:(%d, %d) m:(%d, %d) a:(%d, %d) s:(%d, %d)\n", cMinX, cMaxX, cMinM, cMaxM, cMinA, cMaxA, cMinS, cMaxS);

	while (1)
	{
		for (; curRules[iRule].cmp != -1; iRule++)
		{
			//The last rule has no comparison, and can either accept, reject or send to another workflow
			if (curRules[iRule].cmp == 0)
			{
				if (curRules[iRule].dest[0] == 'R')
				{
					return lTotal;
				}
				if (curRules[iRule].dest[0] == 'A')
				{
					lTotal += (long)(cMaxX - cMinX + 1) * (long)(cMaxM - cMinM + 1) * (long)(cMaxA - cMinA + 1) * (long)(cMaxS - cMinS + 1);
					return lTotal;
				}

				//Follow to the listed rule
				curRules = (RULE *)ht_get(ruletable, curRules[iRule].dest);
				break;
			}
			else
			{
				int bFollowRule = 0;
				if (curRules[iRule].cmp == '<')
				{
					switch(curRules[iRule].partCat)
					{
						case 'x':
							if (cMinX < curRules[iRule].val)
							{
								bFollowRule = 1;
								if (cMaxX >= curRules[iRule].val)
								{
									lTotal += validateAllAccepted(
										ruletable, curRules, iRule + 1, curRules[iRule].val, cMaxX, cMinM, cMaxM, cMinA, cMaxA, cMinS, cMaxS);
									cMaxX = curRules[iRule].val - 1;
								}
							}
							break;
						case 'm':
							if (cMinM < curRules[iRule].val)
							{
								bFollowRule = 1;
								if (cMaxM >= curRules[iRule].val)
								{
									lTotal += validateAllAccepted(
										ruletable, curRules, iRule + 1, cMinX, cMaxX, curRules[iRule].val, cMaxM, cMinA, cMaxA, cMinS, cMaxS);
									cMaxM = curRules[iRule].val - 1;
								}
							}
							break;
						case 'a':
							if (cMinA < curRules[iRule].val)
							{
								bFollowRule = 1;
								if (cMaxA >= curRules[iRule].val)
								{
									lTotal += validateAllAccepted(
										ruletable, curRules, iRule + 1, cMinX, cMaxX, cMinM, cMaxM, curRules[iRule].val, cMaxA, cMinS, cMaxS);
									cMaxA = curRules[iRule].val - 1;
								}
							}
							break;
						case 's':
							if (cMinS < curRules[iRule].val)
							{
								bFollowRule = 1;
								if (cMaxS >= curRules[iRule].val)
								{
									lTotal += validateAllAccepted(
										ruletable, curRules, iRule + 1, cMinX, cMaxX, cMinM, cMaxM, cMinA, cMaxA, curRules[iRule].val, cMaxS);
									cMaxS = curRules[iRule].val - 1;
								}
							}
							break;
					}
				}
				else if (curRules[iRule].cmp == '>')
				{
					switch(curRules[iRule].partCat)
					{
						case 'x':
							if (cMaxX > curRules[iRule].val)
							{
								bFollowRule = 1;
								if (cMinX <= curRules[iRule].val)
								{
									lTotal += validateAllAccepted(
										ruletable, curRules, iRule + 1, cMinX, curRules[iRule].val, cMinM, cMaxM, cMinA, cMaxA, cMinS, cMaxS);
									cMinX = curRules[iRule].val + 1;
								}
							}
							break;
						case 'm':
							if (cMaxM > curRules[iRule].val)
							{
								bFollowRule = 1;
								if (cMinM <= curRules[iRule].val)
								{
									lTotal += validateAllAccepted(
										ruletable, curRules, iRule + 1, cMinX, cMaxX, cMinM, curRules[iRule].val, cMinA, cMaxA, cMinS, cMaxS);
									cMinM = curRules[iRule].val + 1;
								}
							}
							break;
						case 'a':
							if (cMaxA > curRules[iRule].val)
							{
								bFollowRule = 1;
								if (cMinA <= curRules[iRule].val)
								{
									lTotal += validateAllAccepted(
										ruletable, curRules, iRule + 1, cMinX, cMaxX, cMinM, cMaxM, cMinA, curRules[iRule].val, cMinS, cMaxS);
									cMinA = curRules[iRule].val + 1;
								}
							}
							break;
						case 's':
							if (cMaxS > curRules[iRule].val)
							{
								bFollowRule = 1;
								if (cMinS <= curRules[iRule].val)
								{
									lTotal += validateAllAccepted(
										ruletable, curRules, iRule + 1, cMinX, cMaxX, cMinM, cMaxM, cMinA, cMaxA, cMinS, curRules[iRule].val);
									cMinS = curRules[iRule].val + 1;
								}
							}
							break;
					}
				}

				if (bFollowRule)
				{
					if (curRules[iRule].dest[0] == 'R')
					{
						return lTotal;
					}
					if (curRules[iRule].dest[0] == 'A')
					{
						lTotal += (long)(cMaxX - cMinX + 1) * (long)(cMaxM - cMinM + 1) * (long)(cMaxA - cMinA + 1) * (long)(cMaxS - cMinS + 1);
						return lTotal;
					}

					curRules = (RULE *)ht_get(ruletable, curRules[iRule].dest);
					break;
				}
				else
				{
					//Continue to the next rule in the list
					continue;
				}
			}
		}

		iRule = 0;
	}

Return:
	return lTotal;
}

/**
 * Follows the workflows to either accept or reject the given part.
*/
long validatePart(ENTRY **ruletable, PART *parts, int iPart)
{
	PART part = parts[iPart];
	long lRating = 0;

	//Start with the "in" workflow
	RULE *startRules = (RULE *)ht_get(ruletable, "in");
	RULE *curRules = startRules;

	//printf("Validating part x %d\n", part.x);
	while (1)
	{
		for (int iRule = 0; curRules[iRule].cmp != -1; iRule++)
		{
			if (curRules[iRule].cmp == 0)
			{
				if (curRules[iRule].dest[0] == 'R')
				{
					//printf("Rejecting part x %d\n", part.x);
					return 0;
				}
				if (curRules[iRule].dest[0] == 'A')
				{
					//printf("Accepting part x %d\n", part.x);
					return (long)(part.x + part.m + part.a + part.s);
				}

				//printf("Following to workflow %s\n", curRules[iRule].dest);
				curRules = (RULE *)ht_get(ruletable, curRules[iRule].dest);
				break;
			}
			else
			{
				int bFollowRule = 0;
				if (curRules[iRule].cmp == '<')
				{
					switch(curRules[iRule].partCat)
					{
						case 'x':
							if (part.x < curRules[iRule].val)
								bFollowRule = 1;
							break;
						case 'm':
							if (part.m < curRules[iRule].val)
								bFollowRule = 1;
							break;
						case 'a':
							if (part.a < curRules[iRule].val)
								bFollowRule = 1;
							break;
						case 's':
							if (part.s < curRules[iRule].val)
								bFollowRule = 1;
							break;
					}
				}
				else if (curRules[iRule].cmp == '>')
				{
					switch(curRules[iRule].partCat)
					{
						case 'x':
							if (part.x > curRules[iRule].val)
								bFollowRule = 1;
							break;
						case 'm':
							if (part.m > curRules[iRule].val)
								bFollowRule = 1;
							break;
						case 'a':
							if (part.a > curRules[iRule].val)
								bFollowRule = 1;
							break;
						case 's':
							if (part.s > curRules[iRule].val)
								bFollowRule = 1;
							break;
					}
				}

				if (bFollowRule)
				{
					if (curRules[iRule].dest[0] == 'R')
					{
						//printf("Rejecting part x %d\n", part.x);
						return 0;
					}
					if (curRules[iRule].dest[0] == 'A')
					{
						//printf("Accepting part x %d\n", part.x);
						return (long)(part.x + part.m + part.a + part.s);
					}
					
					//printf("Following to workflow %s\n", curRules[iRule].dest);
					curRules = (RULE *)ht_get(ruletable, curRules[iRule].dest);
					break;
				}
				else
				{
					//printf("Continuing to next rule\n");
					continue;
				}
			}
		}

	}

}

//Gets the total sum of all part categories for all parts in the list
long validateParts(ENTRY **ruletable, PART *parts, int iParts)
{
	long lTotRatings = 0;

	for (int i = 0; i < iParts; i++)
	{
		lTotRatings += validatePart(ruletable, parts, i);
	}
	return lTotRatings;
}

/**
 * Given some parts with values of part ratings:
 * {x=787,m=2655,a=1222,s=2876}
 * {x=1679,m=44,a=2067,s=496}
 * 
 * Validate them given some workflows:
 * px{a<2006:qkq,m>2090:A,rfg}
 * pv{a>1716:R,A}
 * 
 * Workflow 'px' has three rules, evaluated left to right.
 * If the part has an 'a' < 2006, continue to workflow 'qkq'. If
 * part category 'm' > 2090, Accept the part. Otherwise, continue to 
 * workflow 'rfg'
 * 
 * Find the sum of all rating values of all accepted parts.
 * 
 * For part two, find the amount of combinations of part ratings that would 
 * result in the part being accepted, if each rating (x,m,a,s) could range 
 * from 1 to 4000, inclusive.
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day19(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;

	PART parts[300];
	int iParts = 0;

	ENTRY **table = ht_create();

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		//Parse a part
		if (pCur[0] == '{')
		{
			if (EOF == sscanf(pCur, "{x=%d,m=%d,a=%d,s=%d}", &parts[iParts].x, &parts[iParts].m, &parts[iParts].a, &parts[iParts].s))
			{
				printf("PART Scanning error %s\n", strerror(errno));
				return;
			}
			iParts++;
		}
		//Parse a workflow
		else if (pCur[0] != '\n')
		{
			char szRules[100] = {0};
			char szName[5] = {0};

			//Weird scanning format: [^{] matches all characters that are not '{'
			if (EOF == sscanf(pCur, "%4[^{]{%[^{}]}\n", szName, szRules))
			{
				printf("Workflow scanning error %s\n", strerror(errno));
				return;
			}

			//printf("Parsing rules %s for %s\n", szRules, szName);
			int iRules = 0;
			RULE *rules = (RULE *)malloc(sizeof(RULE) * MAX_RULES);
			char *tok = strtok(szRules, ",");
			char *dest;
			while (NULL != tok)
			{
				if (NULL != (dest = strchr(tok, ':')))
				{
					sscanf(tok, "%c%c%d:%s", &rules[iRules].partCat, &rules[iRules].cmp, &rules[iRules].val, rules[iRules].dest);
				}
				else
				{
					strcpy(rules[iRules].dest, tok);
					rules[iRules].cmp = 0;
				}
				iRules++;
				tok = strtok(NULL, ",\n");
			}
			rules[iRules].cmp = -1;

			//add the workflow to a hashtable
			ht_add(table, szName, (void *)rules);
		}
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	//Part one
	long lTotRatings = validateParts(table, parts, iParts);	
	
	//Part two
	long lTotAccepted = validateAllAccepted(table, (RULE *)ht_get(table, "in"), 0, 1, 4000, 1, 4000, 1, 4000, 1, 4000);

	printf("Total rating of all accepted parts is %ld\n", lTotRatings);
	printf("Total accepted part configurations from 1 to 4000 is %ld\n", lTotAccepted);
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

	day19(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
