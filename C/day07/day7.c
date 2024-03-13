#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 15
#define MAX_HANDS 1002
#define HAND_SIZE 5

//which part of the day's problem to compute
#define PART 2

//Supported hand types, worst to best
typedef enum handtype 
{
	HIGH_CARD,
	PAIR,
	TWO_PAIR,
	TRIPS,
	FULL_HOUSE,
	QUADS,
	QUINTS
} TYPE;


typedef struct hand
{
	char cards[HAND_SIZE + 1];
	int bid;
	TYPE type;
} HAND;

static int values[90] = {0};

/** 
 * Fill the card value array with individual card ranks
 * We could save some memory by standardizing everything along '2' = 0
 * and lowering the size of values[].
*/ 

void fillValues()
{
	values['2'] = 2;
	values['3'] = 3;
	values['4'] = 4;
	values['5'] = 5;
	values['6'] = 6;
	values['7'] = 7;
	values['8'] = 8;
	values['9'] = 9;
	values['T'] = 10;
	if (PART == 1)
		values['J'] = 11;
	//For part two, 'J' is a wildcard joker with the lowest value of all cards
	else
		values['J'] = 1;
	values['Q'] = 12;
	values['K'] = 13;
	values['A'] = 14;
}

/**
 * Calculate the hand type based on imaginary five-card poker hands:
 * 
 * In order of rank, best to worst:
 * Quints(five of a kind)       : AAAAA
 * Quads(four of a kind)        : AAAA9
 * Full House(trips and a pair) : AAA44
 * Trips(three of a kind)       : AAAQ8
 * Two Pair                     : AA992
 * Pair                         : AA3K9
 * High Card                    : A923J
 * 
 * /////////////////////////////////////////////////////////////////////////////////
 * Part two only:
 * In part two of the problem, 'J' is considered a wildcard joker instead of a Jack.
 * Wildcards can be whatever card is needed to make the best possible hand.
 * 
 * For example:
 *   AAAAJ is quints instead of quads
 *   9922J is a full house instead of two pair
 *   8KQJ2 is a pair instead of high card
 * 
*/
TYPE evaluateHand(char *hand)
{
	int iPairs = 0;
	int bTrips = 0;
	int bQuads = 0;
	int bQuints = 0;
	int aCardAmts[90] = {0};

	//For part 2
	int iNumJokers = 0;

	for (int i = 0; i < HAND_SIZE; i++)
	{
		/* As we parse the hand, calculate what possible hands we have by tracking
		   how many of each card are in the hand and validating what possible hands
		   we have.
		*/

		aCardAmts[hand[i]]++;

		if (2 == PART && hand[i] == 'J')
		{
			iNumJokers++;
		}
		else
		{
			switch (aCardAmts[hand[i]])
			{
				case 1:
					continue;
					break;
				case 2:
					iPairs++;
					break;
				case 3:
					iPairs--;
					bTrips++;
					break;
				case 4:
					bTrips--;
					bQuads++;
					break;
				case 5:
					bQuads--;
					bQuints++;
					break;
				default:
					printf("Error: unexpected amount of card!\n");
					return 0;
					break;
			}
		}
	}

	if (1 == PART || iNumJokers == 0)
	{
		if (bQuints)
			return QUINTS;
		if (bQuads)
			return QUADS;
		if (bTrips)
		{
			if (iPairs)
				return FULL_HOUSE;
			else
				return TRIPS;
		}
		if (2 == iPairs)
			return TWO_PAIR;
		if (1 == iPairs)
			return PAIR;
	
		//Default: no pairs
		return HIGH_CARD;
	}
	else //Part 2
	{
		//Add jokers to the best possible current hand
		switch(iNumJokers)
		{
			case 5:
			case 4:
				return QUINTS;
				break;
			case 3:
				if (iPairs)
					return QUINTS;
				return QUADS;
				break;
			case 2:
				if (bTrips)
					return QUINTS;
				if (iPairs)
					return QUADS;
				return TRIPS;
			case 1:
				if (bQuads)
					return QUINTS;
				if (bTrips)
					return QUADS;
				if (2 == iPairs)
					return FULL_HOUSE;
				if (1 == iPairs)
					return TRIPS;
				
				return PAIR;
		}
	}
}

/**
 * Compare two hands:
 * If the hands are different types, this is a simple comparison.
 * If the types are the same, compare the value of the individual cards in each
 * hand from left to right.
 * 
 * Unlike real poker this means that a hand like 33332 is stronger than 2AAAA
 * because the first card in the first hand (3) is stronger than the first card
 * in the second hand(2)
 * 
 * Return:
 *   negative number if hand1 is worse than hand2
 *   positive number if hand1 is better than hand2
 *   zero if the hands are identical
*/
static int compareHands(const void *hand1, const void *hand2)
{
	HAND *h1 = (HAND * const)hand1;
	HAND *h2 = (HAND * const)hand2;

	if (h1->type != h2->type)
		return h1->type - h2->type;
	
	//If the hands are the same type, compare each card value in order
	for(int i = 0; i < HAND_SIZE; i++)
	{
		if (h1->cards[i] != h2->cards[i])
			return values[h1->cards[i]] - values[h2->cards[i]];
	}

	return 0;
}

/**
 * Given a list of imaginary poker hands and bids, calculate the total winnings of
 * all hands. The winnings of a given hand is its hand rank multiplied by the bid.
 * 
 * 32T3K 765
 * T55J5 684
 * QQQJA 483
 * KK677 28
 * KTJJT 220
 * 
 * (In part one)
 * The worst hand is 32T3K(pair), with winnings of (1 * 765)
 * The best hand is QQQJA(trips), with winnings of (5 * 483) 
 * 
 * (In part two)
 * The worst hand is 32T3K(pair), with winnings of (1 * 765)
 * The best hand is KTJJT(quads), with winnings of (5 * 220)
 * 
 * See above functions for hand calculations and comparison descriptions.
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day7(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;
	unsigned long lTotWinnings = 0;

	int iNumHands = 0;
	HAND aHands[MAX_HANDS];

	fillValues();

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		HAND *CurHand = &aHands[iNumHands];
		sscanf(pCur, "%s %d\n", CurHand->cards, &CurHand->bid);
		
		CurHand->type = evaluateHand(CurHand->cards);
		iNumHands++;
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	//Sort all hands by type/rank
	qsort(aHands, (size_t)iNumHands, sizeof(HAND), compareHands);

	for (int i = 0; i < iNumHands; i++)
	{
		int iHandScore = aHands[i].bid * (i + 1);

		printf("Hand %d(%s) has score %d\n", i + 1, aHands[i].cards, iHandScore);

		lTotWinnings += (unsigned long)iHandScore;
	}

	printf("Total winnings is %lu!\n", lTotWinnings);
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

	day7(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
