#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 23500
#define NUM_BOXES 256

//A lens has a focal length and links to the previous/next lenses in the list
typedef struct lens
{
	int len;
	char label[10];
	struct lens *prev;
	struct lens *next;
} LENS;

//Each box has an id and a doubly-linked list of lenses within it
typedef struct box 
{
	int id;
	LENS *first;
} BOX;

/**
 * Updates a lens with a new length if it exists in the box, or adds the new lens
 * to the end of the lens list if it isn't in the box.
*/
void addOrUpdateLens(BOX *box, char *label, int len)
{
	LENS *cur = box->first;

	while (NULL != cur)
	{
		if (!strcmp(label, cur->label))
		{
			//found an existing lens!
			cur->len = len;
			return;
		}

		if (cur->next == NULL)
			break;
		cur = cur->next;
	}

	//If not found, allocate and add a lens
	LENS *lens = (LENS *)calloc(1, sizeof(LENS));
	lens->len = len;
	strcpy(lens->label, label);
	lens->prev = cur;

	if (NULL == cur)
		box->first = lens;
	else
		cur->next = lens;
}

/**
 * Removes a lens from the box(if it's there).
*/
void removeLens(BOX *box, char *label)
{
	LENS *cur = box->first;

	while (NULL != cur)
	{
		if (!strcmp(label, cur->label))
		{
			//found the lens!
			LENS *save = cur->prev;
			
			//If this is the only lens, the box is now empty
			if (NULL == cur->prev && NULL == cur->next)
			{
				box->first = NULL;
				free(cur);
				return;
			}
			
			if (NULL != cur->prev)
				cur->prev->next = cur->next;
			else 
				//The box was empty, so add this lens to the front
				box->first = cur->next;

			if (NULL != cur->next)
				cur->next->prev = save;

			free(cur);
			return;
		}

		cur = cur->next;
	}
}

/**
 * Calculates the sum of the powers of each lens in the box.
 * 
 * The power of a given lens is the product of three numbers:
 * - 1 plus the box id
 * - The position of the lens in the box (the first lens is 1, second is 2, etc)
 * - The focal length of the lens
*/
long countPower(BOX *box)
{
	LENS *cur = box->first;
	int id = box->id;
	int iLensNum = 1;
	long lPower = 0;

	while (NULL != cur)
	{
		long pow = id + 1;
		pow *= iLensNum;
		pow *= cur->len;
		lPower += pow;

		cur = cur->next;
		iLensNum++;
	}

	//if (lPower != 0)
		//printf("Power of box %d is %ld\n", id, lPower);
	return lPower;
}

//Pretty prints the box and its lenses
printBox(BOX *box)
{
	LENS *cur = box->first;
	int iLensNum = 1;

	if (cur != NULL)
		printf("printing box %d\n", box->id);
	while (NULL != cur)
	{
		printf("\t%d: Lens %s power %d\n", iLensNum, cur->label, cur->len);
		cur = cur->next;
		iLensNum++;
	}
}

/**
 * Given a list of actions to execute on a list of 256 boxes, calculate the total
 * power of every lens in each box after executing all actions.
 * 
 * Example actions:
 *   cm=2
 *      'cm' is the label of the lens to work with.
 *      '=' indicates that this lens should be added to a box or updated if it's in the box
 *      '2' is the focal length of the lens 
 *   qch-
 *      'qch' is the label of the lens to work with.
 *      '-' indicates that this lens should be removed from the box if it exists
 * 
 * For all actions, the box id to work with is calculated by hashing the lens label:
 *   For each character in the label:
 *   - Add the ascii code of the character to the hash
 *   - Multiply the hash by 17
 *   - Set the hash to the remainder of dividing itself by 256
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day15(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;
	char *tokptr = NULL;
	char *szToken = NULL;

	long lTotalPower = 0;

	BOX *boxes = (BOX *)calloc(NUM_BOXES, sizeof(BOX));
	for (int i = 0; i < NUM_BOXES; i++)
	{
		boxes[i].id = i;
	}
	char label[10] = { 0 };

	//parse the line
	pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile);

	szToken = strtok_r(pCur, ",", &tokptr);
	while (szToken != NULL)
	{
		long hash = 0;
		int ch = 0;
		char c = *szToken;

		//Calculate the hash of the lens label and save the label string
		while (c != '=' && c != '-')
		{
			hash += (long)c;
			hash *= 17;
			hash %= 256;
			label[ch++] = c;

			c = *++szToken;
		}

		//For '=', update the lens in the corresponding box or add it at the end
		//of the box's list of lenses if it isn't there
		if (c == '=')
		{
			szToken++;
			int len = atoi(szToken);
			//printf("Setting box %ld lens %s to %d\n", hash, label, len);
			addOrUpdateLens(&(boxes[hash]), label, len);
		}
		//For '-', remove the lens from the box if it exists
		else if (c == '-')
		{
			//printf("Removing %s\n", label);
			removeLens(&(boxes[hash]), label);
		}

		memset(label, 0, 10);
		szToken = strtok_r(NULL, ",", &tokptr);
	}

	//Count the power of the lenses in each box
	for (int i = 0; i < NUM_BOXES; i++)
	{
		//printBox(&(boxes[i]));
		lTotalPower += countPower(&(boxes[i]));
	}
	printf("Done, total power is %ld\n", lTotalPower);
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

	day15(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
