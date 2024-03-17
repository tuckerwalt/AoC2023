#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include "simplehash.h"

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 300
#define DAY_PART 2

//See simplehash.c/simplehash.h for definitions of a NODE and a hashtable to support them

/**
 * Given a hashtable of nodes and a list of directions, find the number of hops
 * required to get from node "AAA" to "ZZZ"
*/
void part1(NODE **table, char *szDirections, int iNumDirs)
{
	int iCurDir = 0;
	int iNumHops = 0;
	NODE *pCurNode = ht_get(table, "AAA");

	printf("Starting at %s\n", pCurNode->id);
	while (strcmp(pCurNode->id, "ZZZ"))
	{
		if (szDirections[iCurDir] == 'L')
			pCurNode = pCurNode->left;
		else if (szDirections[iCurDir] == 'R')
			pCurNode = pCurNode->right;
		else
		{
			printf("ERROR unexpected direction %c\n", szDirections[iCurDir]);
			return;
		}

		if (NULL == pCurNode)
		{
			printf("ERROR hit a null node!!!\n");
			return;
		}

		iNumHops++;
		iCurDir++;
		if (iCurDir == iNumDirs)
			iCurDir = 0;
	}

	printf("Part one: Number of hops to get to ZZZ is %d\n", iNumHops);
}

/**
 * Lazy least-common-multiple implementation
 * Start testing with the highest number, incrementing by that number.
*/
unsigned long lcm(unsigned long *nums, int iNums) {
	unsigned long max = 0;
	unsigned long increment = 0;

	for (int i = 0; i < iNums; i++)
	{
		if (nums[i] > max )
			max = nums[i];
	}

	increment = max;
	for (int i = 0; i < iNums; i++)
	{
		//Once we hit a non-multiple, increment and start over with the first num
		if (max % nums[i] != 0)
		{
			max += increment;
			i = -1;
		}
	}
    
	return max;
}

/**
 * Given a node ending in 'A', returns the number of steps taken to reach a node
 * ending in 'Z' 
*/
int findZ(NODE **table, char *szDirections, int iNumDirs, NODE *startNode)
{
	int iCurDir = 0;
	int iNumHops = 0;
	int iNumHopsLoop = 0;
	NODE *node = startNode;

	while (node->type != Z)
	{
		if (szDirections[iCurDir] == 'L')
			node = node->left;
		else if (szDirections[iCurDir] == 'R')
			node = node->right;
		else
		{
			printf("ERROR unexpected direction %c\n", szDirections[iCurDir]);
			return 1;
		}

		if (NULL == node)
		{
			printf("ERROR hit a null node!!!\n");
			return 1;
		}

		iNumHops++;
		iCurDir++;
		if (iCurDir == iNumDirs)
			iCurDir = 0;
	}

	return iNumHops;
}

/**
 * Given a hashtable of nodes, a list of directions to follow, and a list of type A nodes,
 * determine how many hops are required for ALL nodes to reach a type Z node at the same time.
 * 
 * For each starting node, find out how many hops it takes to get to a Z node. To reach all 
 * Z nodes at the same time, it'll be the LCM of all node lengths. This is due to the paths
 * being cyclic and each starter node only ever reaching one kind of Z node, although
 * this is not indicated well in the problem description.
 * 
*/
void part2(NODE **table, char *szDirections, int iNumDirs, NODE **p2FollowNodes, int iFollowNodes)
{
	int iCurDir = 0;
	int iNumHops = 0;
	int bAllZ = 0;
	
	unsigned long *cyclelens = (unsigned long *)malloc(iFollowNodes * sizeof(unsigned long));

	for (int i = 0; i < iFollowNodes; i++)
	{
		cyclelens[i] = (unsigned long) findZ(table, szDirections, iNumDirs, p2FollowNodes[i]);
		printf("%s found Z node in %d steps\n", p2FollowNodes[i]->id, cyclelens[i]);
	}

	//Find the LCM of all cycles
	unsigned long LCM = lcm(cyclelens, iFollowNodes);
	printf("Part two: ALL nodes hit Z in %lu hops\n", LCM);
}

/**
 * We're given a list of nodes: each node has an identifier, a connecting left node 
 * and connecting right node. At the top is a list of directions to follow, left or right:
 * 
 * LLR
 *
 * AAA = (BBB, BBB)
 * BBB = (AAA, ZZZ)
 * ZZZ = (ZZZ, ZZZ)
 * 
 * Start at "AAA" and follow the L/R instructions(repeating) until reaching "ZZZ",
 * counting how many hops are required. The above example requires 6 hops.
 * 
 * For part two, follow ALL nodes that end in the letter 'A' and count the hops required
 * for all nodes to be at a node ending in 'Z' AT THE SAME TIME.
 * 
 * LR
 * 
 * 11A = (11B, XXX)
 * 11B = (XXX, 11Z)
 * 11Z = (11B, XXX)
 * 22A = (22B, XXX)
 * 22B = (22C, 22C)
 * 22C = (22Z, 22Z)
 * 22Z = (22B, 22B)
 * XXX = (XXX, XXX)
 * 
 * Starting at 11A and 22A, it takes 6 hops for the nodes to be at 11Z and 22Z, respectively.
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day8(FILE *pFile, int part)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char szDirections[LINE_BUFFER_SIZE] = { 0 };
	int iNumDirs = 0;
	char *pCur = NULL;
	char c = 0;
	
	char id[4] = {0};
	char left[4] = {0};
	char right[4] = {0};

	NODE **p2FollowNodes; 
	int iFollowNodes = 0;
	int max_follownodes = 5;
	if (2 == part)
		p2FollowNodes = (NODE **)malloc(sizeof(NODE *) * max_follownodes);

	NODE **table = ht_create();
	if (NULL == table)
	{
		printf("ERROR: failed to allocate hash table!\n");
		return;
	}

	//Read the directions
	pCur = fgets(szDirections, LINE_BUFFER_SIZE, pFile);
	if (NULL == pCur)
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}
	iNumDirs = strlen(szDirections);
	if (szDirections[iNumDirs - 1] == '\n')
		iNumDirs -= 1;

	//parse nodes
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		if (pCur[0] == '\n')
		{
			continue;
		}

		if (EOF == sscanf(pCur, "%3s = (%3s, %3s)\n", id, left, right))
		{
			printf("ERROR: failed to scan node line %s\n", pCur);
			return;
		}

		NODE *added = ht_add_or_update(table, id, left, right);

		//For part 2, add all nodes ending in 'A' to the tracking list
		if (2 == part)
		{
			if (added->type == A)
			{
				if (iFollowNodes == max_follownodes)
				{
					max_follownodes *= 2;
					p2FollowNodes = (NODE **)reallocarray(p2FollowNodes, max_follownodes, sizeof(NODE *));
				}

				p2FollowNodes[iFollowNodes++] = added;
			}
		}
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	if (1 == part)
		part1(table, szDirections, iNumDirs);
	else if (2 == part)
		part2(table, szDirections, iNumDirs, p2FollowNodes, iFollowNodes);
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

	day8(pFile, DAY_PART);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
