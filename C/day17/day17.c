#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include "simplehash.h"
#include "day17.h"
#include "minheap.h"
#include "time.h"

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 150
#define MAX_ACCEPTABLE_WEIGHT 1000

typedef struct block
{
	int heat;
	int weight[4][3];
} BLOCK;

void addUnvisited(NODE **unv, int *iUnv, int *iMaxUnv, int row, int col, DIR direction, int sequencelen, int heat, int weight, int visited, int *bReplace, int index)
{
	int ind = *bReplace ? index : *iUnv;

	if (*iUnv == *iMaxUnv)
	{
		(*iMaxUnv) *= 2;
		*unv = (NODE *)realloc(*unv, *iMaxUnv * sizeof(NODE));
	}

	(*unv)[ind].row = row;
	(*unv)[ind].col = col;
	(*unv)[ind].direction = direction;
	(*unv)[ind].sequencelen = sequencelen;
	(*unv)[ind].weight = weight;

	if (*bReplace)
	{
		*bReplace = 0;
	}	
	else
	{
		(*iUnv)++;
	}
}

NODE *getNextNode(NODE *unvisited, int iUnv, NODE **table, int *retindex)
{
	//printf("%d unvisited to scan\n", iUnv);
	int lowestweight = __INT_MAX__;
	int lowind = 0;
	for (int i = 0; i < iUnv; i++)
	{
		if (unvisited[i].weight < lowestweight)
		{
			lowestweight = unvisited[i].weight;
			lowind = i;
		}
		
	}
	*retindex = lowind;
	return ht_get(table, unvisited[lowind].row, unvisited[lowind].col,
		unvisited[lowind].direction, unvisited[lowind].sequencelen);
}

void traverseDij(int **map, int iDim)
{
	int iMaxUnvisited = 50;
	int iNumUnvisited = 0;
	NODE **table = ht_create();
	NODE *unvisited = (NODE *)malloc(iMaxUnvisited * sizeof(NODE));
	int iSeq = -1;
	int bReplace = 0;

	for (int dir = 0; dir < 4; dir++)
	{
		for (int seq = 0; seq < 3; seq++)
		{
			ht_add(table, 0, 0, dir, seq, map[0][0], 0, 1);
		}
	}
	ht_add(table, 1, 0, DOWN, 0, map[1][0], map[1][0], 0);
	ht_add(table, 0, 1, RIGHT, 0, map[0][1], map[0][1], 0);
	addUnvisited(&unvisited, &iNumUnvisited, &iMaxUnvisited, 1, 0, DOWN, 0, map[1][0], map[1][0], 0, &bReplace, 0);
	addUnvisited(&unvisited, &iNumUnvisited, &iMaxUnvisited, 0, 1, RIGHT, 0, map[0][1], map[0][1], 0, &bReplace, 1);
	
	int curIndex = 0;
	NODE *current = getNextNode(unvisited, iNumUnvisited, table, &curIndex);
	if (NULL == current)
	{
		printf("CURRENT NULL NODE???\n");
		return;
	}
	//printf("Current node (%d %d), dir is %d sequence is %d weight is %d\n", current->row, current->col, current->direction, current->sequencelen, current->weight);

	while (current->row != iDim - 1 || current->col != iDim - 1)
	{
		bReplace = 1;

		DIR dir = current->direction;
		int seq = current->sequencelen;
		int row = current->row;
		int col = current->col;

		printf("Current node (%d %d), dir is %d sequence is %d weight is %d\n", current->row, current->col, current->direction, current->sequencelen, current->weight);
		//DOWN
		if (row < iDim - 1 && dir != UP)
		{
			//nextBlock = &map[row+1][col];
			if (!(dir == DOWN && seq == 2) /*&&
				uWeight + nextBlock->heat  < nextBlock->weight*/)
			{
				int newr = row+1;
				int newc = col;
				int streak = dir == DOWN ? seq + 1 : 0;
				NODE *new = ht_get(table, newr, newc, DOWN, streak);
				
				if (new == NULL)
				{
					int heat = map[newr][newc];
					ht_add(table, newr, newc, DOWN, streak, heat, current->weight + heat, 0);
					addUnvisited(&unvisited, &iNumUnvisited, &iMaxUnvisited, newr, newc, DOWN, streak, heat, current->weight + heat, 0, &bReplace, curIndex);
				}
				else
				{
					if (new->weight > current->weight + new->heat)
						new->weight = current->weight + new->heat;
				}
			}
		}

		//UP
		if (row > 0 && dir != DOWN)
		{
			//nextBlock = &map[row-1][col];
			if (!(dir == UP && seq == 2) /*&&
				uWeight + nextBlock->heat  < nextBlock->weight*/)
			{
				int newr = row-1;
				int newc = col;
				int streak = dir == UP ? seq + 1 : 0;
				NODE *new = ht_get(table, newr, newc, UP, streak);
				
				if (new == NULL)
				{
					int heat = map[newr][newc];
					ht_add(table, newr, newc, UP, streak, heat, current->weight + heat, 0);
					addUnvisited(&unvisited, &iNumUnvisited, &iMaxUnvisited, newr, newc, UP, streak, heat, current->weight + heat, 0, &bReplace, curIndex);
				}
				else
				{
					if (new->weight > current->weight + new->heat)
						new->weight = current->weight + new->heat;
				}
			}
		}

		//LEFT
		if (col > 0 && dir != RIGHT)
		{
			//nextBlock = &map[row][col-1];
			if (!(dir == LEFT && seq == 2) /*&&
				uWeight + nextBlock->heat  < nextBlock->weight*/)
			{
				int newr = row;
				int newc = col-1;
				int streak = dir == LEFT ? seq + 1 : 0;
				NODE *new = ht_get(table, newr, newc, LEFT, streak);
				
				if (new == NULL)
				{
					int heat = map[newr][newc];
					ht_add(table, newr, newc, LEFT, streak, heat, current->weight + heat, 0);
					addUnvisited(&unvisited, &iNumUnvisited, &iMaxUnvisited, newr, newc, LEFT, streak, heat, current->weight + heat, 0, &bReplace, curIndex);
				}
				else
				{
					if (new->weight > current->weight + new->heat)
						new->weight = current->weight + new->heat;
				}
			}
		}

		//RIGHT
		if (col < iDim - 1 && dir != LEFT)
		{
			//nextBlock = &map[row][col+1];
			if (!(dir == RIGHT && seq == 2) /*&&
				uWeight + nextBlock->heat  < nextBlock->weight*/)
			{
				int newr = row;
				int newc = col+1;
				int streak = dir == RIGHT ? seq + 1 : 0;
				NODE *new = ht_get(table, newr, newc, RIGHT, streak);
				
				if (new == NULL)
				{
					int heat = map[newr][newc];
					ht_add(table, newr, newc, RIGHT, streak, heat, current->weight + heat, 0);
					addUnvisited(&unvisited, &iNumUnvisited, &iMaxUnvisited, newr, newc, RIGHT, streak, heat, current->weight + heat, 0, &bReplace, curIndex);
				}
				else
				{
					if (new->weight > current->weight + new->heat)
						new->weight = current->weight + new->heat;
				}
			}
		}

		if (bReplace)
		{
			unvisited[curIndex].weight = 999999;
		}
		current = getNextNode(unvisited, iNumUnvisited, table, &curIndex);
	}

	printf("Done! final weight is %d\n", current->weight);
}

void traverseDijUltra(int **map, int iDim)
{
	int iMaxUnvisited = 50;
	int iNumUnvisited = 0;
	NODE **table = ht_create();
	NODE *unvisited = (NODE *)malloc(iMaxUnvisited * sizeof(NODE));
	int iSeq = -1;
	int bReplace = 0;
	static int checkedNodes = 0;

	for (int dir = 0; dir < 4; dir++)
	{
		for (int seq = 3; seq < 10; seq++)
		{
			ht_add(table, 0, 0, dir, seq, map[0][0], 0, 1);
		}
	}
	ht_add(table, 4, 0, DOWN, 3, map[4][0], map[1][0] + map[2][0] + map[3][0] + map[4][0], 0);
	ht_add(table, 0, 4, RIGHT, 3, map[0][4], map[0][1] + map[0][2] + map[0][3] + map[0][4], 0);
	addUnvisited(&unvisited, &iNumUnvisited, &iMaxUnvisited, 4, 0, DOWN, 3, map[4][0], map[1][0] + map[2][0] + map[3][0] + map[4][0], 0, &bReplace, 0);
	addUnvisited(&unvisited, &iNumUnvisited, &iMaxUnvisited, 0, 4, RIGHT, 3, map[0][4], map[0][1] + map[0][2] + map[0][3] + map[0][4], 0, &bReplace, 1);
	
	int curIndex = 0;
	NODE *current = getNextNode(unvisited, iNumUnvisited, table, &curIndex);
	if (NULL == current)
	{
		printf("CURRENT NULL NODE???\n");
		return;
	}
	//printf("Current node (%d %d), dir is %d sequence is %d weight is %d\n", current->row, current->col, current->direction, current->sequencelen, current->weight);

	while (current->row != iDim - 1 || current->col != iDim - 1)
	{
		bReplace = 1;

		DIR dir = current->direction;
		int seq = current->sequencelen;
		int row = current->row;
		int col = current->col;

		int newr;
		int newc;
		int newstreak;
		NODE *next;
		int newweight;
		int newheat;

		int bCheckNewNode = 1;

		//printf("Current node (%d %d), dir is %d sequence is %d weight is %d, checked node #%d\n", current->row, current->col, current->direction, current->sequencelen, current->weight, ++checkedNodes);
		//DOWN
		bCheckNewNode = 0;
		if (dir != UP)
		{
			if (dir != DOWN)
			{
				if (row < iDim - 4)
				{
					bCheckNewNode = 1;
					newr = row + 4;
					newc = col;
					newstreak = 3;
					newweight = current->weight + 
								map[row+1][col] + 
								map[row+2][col] +
								map[row+3][col] +
								map[row+4][col];
					newheat = map[newr][newc];
				}
			}
			else if (row < iDim - 1)
			{
				if (seq != 9)
				{
					bCheckNewNode = 1;
					newr = row + 1;
					newc = col;
					newstreak = seq + 1;
					newheat = map[newr][newc];
					newweight = current->weight + newheat;
				}
			}
			if (bCheckNewNode)
			{
				next = ht_get(table, newr, newc, DOWN, newstreak);
				if (next == NULL)
				{
					ht_add(table, newr, newc, DOWN, newstreak, newheat, newweight, 0);
					addUnvisited(&unvisited, &iNumUnvisited, &iMaxUnvisited, newr, newc, DOWN, newstreak, newheat, newweight, 0, &bReplace, curIndex);
				}
				else
				{
					if (next->weight > newweight)
						next->weight = newweight;
				}
			}
		}

		//UP
		bCheckNewNode = 0;
		if (dir != DOWN)
		{
			if (dir != UP)
			{
				if (row > 3)
				{
					bCheckNewNode = 1;
					newr = row - 4;
					newc = col;
					newstreak = 3;
					newweight = current->weight + 
								map[row-1][col] + 
								map[row-2][col] +
								map[row-3][col] +
								map[row-4][col];
					newheat = map[newr][newc];
				}
			}
			else if (row > 0)
			{
				if (seq != 9)
				{
					bCheckNewNode = 1;
					newr = row - 1;
					newc = col;
					newstreak = seq + 1;
					newheat = map[newr][newc];
					newweight = current->weight + newheat;
				}
			}
			if (bCheckNewNode)
			{
				next = ht_get(table, newr, newc, UP, newstreak);
				if (next == NULL)
				{
					ht_add(table, newr, newc, UP, newstreak, newheat, newweight, 0);
					addUnvisited(&unvisited, &iNumUnvisited, &iMaxUnvisited, newr, newc, UP, newstreak, newheat, newweight, 0, &bReplace, curIndex);
				}
				else
				{
					if (next->weight > newweight)
						next->weight = newweight;
				}
			}
		}

		//LEFT
		bCheckNewNode = 0;
		if (dir != RIGHT)
		{
			if (dir != LEFT)
			{
				if (col > 3)
				{
					bCheckNewNode = 1;
					newr = row;
					newc = col - 4;
					newstreak = 3;
					newweight = current->weight + 
								map[row][col-1] + 
								map[row][col-2] +
								map[row][col-3] +
								map[row][col-4];
					newheat = map[newr][newc];
				}
			}
			else if (col > 0)
			{
				if (seq != 9)
				{
					bCheckNewNode = 1;
					newr = row;
					newc = col - 1;
					newstreak = seq + 1;
					newheat = map[newr][newc];
					newweight = current->weight + newheat;
				}
			}
			if (bCheckNewNode)
			{
				next = ht_get(table, newr, newc, LEFT, newstreak);
				if (next == NULL)
				{
					ht_add(table, newr, newc, LEFT, newstreak, newheat, newweight, 0);
					addUnvisited(&unvisited, &iNumUnvisited, &iMaxUnvisited, newr, newc, LEFT, newstreak, newheat, newweight, 0, &bReplace, curIndex);
				}
				else
				{
					if (next->weight > newweight)
						next->weight = newweight;
				}
			}
		}

		//RIGHT
		bCheckNewNode = 0;
		if (dir != LEFT)
		{
			if (dir != RIGHT)
			{
				if (col < iDim - 4)
				{
					bCheckNewNode = 1;
					newr = row;
					newc = col + 4;
					newstreak = 3;
					newweight = current->weight + 
								map[row][col+1] + 
								map[row][col+2] +
								map[row][col+3] +
								map[row][col+4];
					newheat = map[newr][newc];
				}
			}
			else if (col < iDim - 1)
			{
				if (seq != 9)
				{
					bCheckNewNode = 1;
					newr = row;
					newc = col + 1;
					newstreak = seq + 1;
					newheat = map[newr][newc];
					newweight = current->weight + newheat;
				}
			}
			if (bCheckNewNode)
			{
				next = ht_get(table, newr, newc, RIGHT, newstreak);
				if (next == NULL)
				{
					ht_add(table, newr, newc, RIGHT, newstreak, newheat, newweight, 0);
					addUnvisited(&unvisited, &iNumUnvisited, &iMaxUnvisited, newr, newc, RIGHT, newstreak, newheat, newweight, 0, &bReplace, curIndex);
				}
				else
				{
					if (next->weight > newweight)
						next->weight = newweight;
				}
			}
		}

		if (bReplace)
		{
			unvisited[curIndex].weight = 999999;
		}
		current = getNextNode(unvisited, iNumUnvisited, table, &curIndex);
	}

	printf("Done! final weight is %d\n", current->weight);
}

int manhattan(int row, int col, int iDim)
{
	int manhattan = (iDim - 1 - row) + (iDim - 1 - col);
	/*if (manhattan > 150)
		return manhattan * 2.5;
	if (manhattan > 75)
		return manhattan * 2;
	if (manhattan > 30)
		return manhattan * 1.5;
	*/

	//if (manhattan < 10)
	//	return manhattan;
	//if (manhattan < 30)
	//	return manhattan * 1.5;
	return manhattan * 3;
	//return 0;
}

void traverseDijUltraHeap(int **map, int iDim)
{
	int iMaxUnvisited = 50;
	int iNumUnvisited = 0;
	NODE **table = ht_create();
	MINHEAP *unvisited = mh_create();
	int iSeq = -1;
	static int checkedNodes = 0;

	for (int dir = 0; dir < 4; dir++)
	{
		for (int seq = 3; seq < 10; seq++)
		{
			ht_add(table, 0, 0, dir, seq, map[0][0], 0, 1);
		}
	}
	NODE *first = ht_add(table, 4, 0, DOWN, 3, map[4][0], map[1][0] + map[2][0] + map[3][0] + map[4][0], 0);
	NODE *second = ht_add(table, 0, 4, RIGHT, 3, map[0][4], map[0][1] + map[0][2] + map[0][3] + map[0][4], 0);
	mh_insert(unvisited, manhattan(first->row, first->col, iDim) + first->weight, first);
	mh_insert(unvisited, manhattan(second->row, second->col, iDim) + second->weight, second);
	
	int curIndex = 0;
	NODE *current = mh_pop(unvisited);
	NODE *added = NULL;
	if (NULL == current)
	{
		printf("CURRENT NULL NODE???\n");
		return;
	}
	//printf("Current node (%d %d), dir is %d sequence is %d weight is %d\n", current->row, current->col, current->direction, current->sequencelen, current->weight);

	//Continue until we hit the destination
	while (current->row != iDim - 1 || current->col != iDim - 1)
	{
		DIR dir = current->direction;
		int seq = current->sequencelen;
		int row = current->row;
		int col = current->col;

		int newr;
		int newc;
		int newstreak;
		NODE *next;
		int newweight;
		int newheat;

		int bCheckNewNode = 1;

		//printf("Current node (%d %d), dir is %d sequence is %d weight is %d(%d), node check #%d\n", current->row, current->col, current->direction, current->sequencelen, current->weight, manhattan(current->row, current->col, iDim) + current->weight, ++checkedNodes);
		//DOWN
		bCheckNewNode = 0;
		if (dir != UP)
		{
			if (dir != DOWN)
			{
				if (row < iDim - 4)
				{
					bCheckNewNode = 1;
					newr = row + 4;
					newc = col;
					newstreak = 3;
					newweight = current->weight + 
								map[row+1][col] + 
								map[row+2][col] +
								map[row+3][col] +
								map[row+4][col];
					newheat = map[newr][newc];
				}
			}
			else if (row < iDim - 1)
			{
				if (seq != 9)
				{
					bCheckNewNode = 1;
					newr = row + 1;
					newc = col;
					newstreak = seq + 1;
					newheat = map[newr][newc];
					newweight = current->weight + newheat;
				}
			}
			if (bCheckNewNode)
			{
				next = ht_get(table, newr, newc, DOWN, newstreak);
				if (next == NULL)
				{
					added = ht_add(table, newr, newc, DOWN, newstreak, newheat, newweight, 0);
					mh_insert(unvisited, manhattan(added->row, added->col, iDim) + added->weight, added);
				}
				else
				{
					if (next->weight > newweight)
						next->weight = newweight;
				}
			}
		}

		//UP
		bCheckNewNode = 0;
		if (dir != DOWN)
		{
			if (dir != UP)
			{
				if (row > 3)
				{
					bCheckNewNode = 1;
					newr = row - 4;
					newc = col;
					newstreak = 3;
					newweight = current->weight + 
								map[row-1][col] + 
								map[row-2][col] +
								map[row-3][col] +
								map[row-4][col];
					newheat = map[newr][newc];
				}
			}
			else if (row > 0)
			{
				if (seq != 9)
				{
					bCheckNewNode = 1;
					newr = row - 1;
					newc = col;
					newstreak = seq + 1;
					newheat = map[newr][newc];
					newweight = current->weight + newheat;
				}
			}
			if (bCheckNewNode)
			{
				next = ht_get(table, newr, newc, UP, newstreak);
				if (next == NULL)
				{
					added = ht_add(table, newr, newc, UP, newstreak, newheat, newweight, 0);
					mh_insert(unvisited, manhattan(added->row, added->col, iDim) + added->weight, added);
				}
				else
				{
					if (next->weight > newweight)
						next->weight = newweight;
				}
			}
		}

		//LEFT
		bCheckNewNode = 0;
		if (dir != RIGHT)
		{
			if (dir != LEFT)
			{
				if (col > 3)
				{
					bCheckNewNode = 1;
					newr = row;
					newc = col - 4;
					newstreak = 3;
					newweight = current->weight + 
								map[row][col-1] + 
								map[row][col-2] +
								map[row][col-3] +
								map[row][col-4];
					newheat = map[newr][newc];
				}
			}
			else if (col > 0)
			{
				if (seq != 9)
				{
					bCheckNewNode = 1;
					newr = row;
					newc = col - 1;
					newstreak = seq + 1;
					newheat = map[newr][newc];
					newweight = current->weight + newheat;
				}
			}
			if (bCheckNewNode)
			{
				next = ht_get(table, newr, newc, LEFT, newstreak);
				if (next == NULL)
				{
					added = ht_add(table, newr, newc, LEFT, newstreak, newheat, newweight, 0);
					mh_insert(unvisited, manhattan(added->row, added->col, iDim) + added->weight, added);
				}
				else
				{
					if (next->weight > newweight)
						next->weight = newweight;
				}
			}
		}

		//RIGHT
		bCheckNewNode = 0;
		if (dir != LEFT)
		{
			if (dir != RIGHT)
			{
				if (col < iDim - 4)
				{
					bCheckNewNode = 1;
					newr = row;
					newc = col + 4;
					newstreak = 3;
					newweight = current->weight + 
								map[row][col+1] + 
								map[row][col+2] +
								map[row][col+3] +
								map[row][col+4];
					newheat = map[newr][newc];
				}
			}
			else if (col < iDim - 1)
			{
				if (seq != 9)
				{
					bCheckNewNode = 1;
					newr = row;
					newc = col + 1;
					newstreak = seq + 1;
					newheat = map[newr][newc];
					newweight = current->weight + newheat;
				}
			}
			if (bCheckNewNode)
			{
				next = ht_get(table, newr, newc, RIGHT, newstreak);
				if (next == NULL)
				{
					added = ht_add(table, newr, newc, RIGHT, newstreak, newheat, newweight, 0);
					mh_insert(unvisited, manhattan(added->row, added->col, iDim) + added->weight, added);
				}
				else
				{
					if (next->weight > newweight)
						next->weight = newweight;
				}
			}
		}
		current = mh_pop(unvisited);
	}

	printf("Done! final weight is %d\n", current->weight);
}


int lessWeight(BLOCK *blk, DIR dir, int streak, int weight)
{
	if (weight + blk->heat > MAX_ACCEPTABLE_WEIGHT)
		return 0;
	if (blk->weight[dir][streak] > weight + blk->heat)
	{
		//printf("less weight!\n");
		return 1;
	}
	//printf("not less weight, %d %d\n", blk->weight[dir][streak], weight + blk->heat);
	return 0;
}

void traverse(BLOCK **map, int iDim, int row, int col, int uWeight, DIR dir, int iDirStreak)
{
	if ((row < 0 || row >= iDim) ||
		(col < 0 || col >= iDim))
	{
		return;
	}

	BLOCK *nextBlock;
	//printf("Entering block (%d, %d), heat is %d weight is %d\n", row, col, map[row][col].heat, map[row][col].weight[dir][iDirStreak]);


	//DOWN
	if (row < iDim - 1 && dir != UP)
	{
		nextBlock = &map[row+1][col];
		if (!(dir == DOWN && iDirStreak == 2))
		{
			int streak = dir == DOWN ? iDirStreak + 1 : 0;
			if (lessWeight(nextBlock, DOWN, streak, uWeight))
			{
				nextBlock->weight[DOWN][streak] = uWeight + nextBlock->heat;
				traverse(map, iDim, row + 1, col, nextBlock->weight[DOWN][streak], DOWN, streak);
			}
		}
	}

	//UP
	if (row > 0 && dir != DOWN)
	{
		nextBlock = &map[row-1][col];
		if (!(dir == UP && iDirStreak == 2))
		{
			int streak = dir == UP ? iDirStreak + 1 : 0;
			if (lessWeight(nextBlock, UP, streak, uWeight))
			{
				nextBlock->weight[UP][streak] = uWeight + nextBlock->heat;
				traverse(map, iDim, row - 1, col, nextBlock->weight[UP][streak], UP, streak);
			}
		}
	}

	//LEFT
	if (col > 0 && dir != RIGHT)
	{
		nextBlock = &map[row][col-1];
		if (!(dir == LEFT && iDirStreak == 2))
		{
			int streak = dir == LEFT ? iDirStreak + 1 : 0;
			if (lessWeight(nextBlock, LEFT, streak, uWeight))
			{
				nextBlock->weight[LEFT][streak] = uWeight + nextBlock->heat;
				traverse(map, iDim, row, col - 1, nextBlock->weight[LEFT][streak], LEFT, streak);
			}
		}
	}

	//RIGHT
	if (col < iDim - 1 && dir != LEFT)
	{
		nextBlock = &map[row][col+1];
		if (!(dir == RIGHT && iDirStreak == 2))
		{
			int streak = dir == RIGHT ? iDirStreak + 1 : 0;
			if (lessWeight(nextBlock, RIGHT, streak, uWeight))
			{
				nextBlock->weight[RIGHT][streak] = uWeight + nextBlock->heat;
				traverse(map, iDim, row, col + 1, nextBlock->weight[RIGHT][streak], RIGHT, streak);
			}
		}
	}
}

/**
 * Given a 2d map:
 * 2413432
 * 3215453
 * 3255245
 * 3446585
 * 4546657
 * 1438598
 * 4457876
 * 
 * Find the shortest path(in terms of heat loss) starting from the top left 
 * corner and ending at the bottom right corner. Travelling through a given 
 * index causes a "heat loss" equal to the value at that index. Furthermore, 
 * you can only travel at most 3 tiles in a single direction before having to turn.
 * 
 * For part two, you must travel a minimum of four tiles in one direction 
 * and a maximum of 10 tiles before turning.
 * 
 * I'm leaving all of my different iterations of this solution in here:
 * traverse: a Dij-lite recursive traversal finding the minimum heat-loss to get to 
 * 			all of the tiles in the map. To cut down on computations, I use a 
 * 			hardcoded maximum heat-loss before short-circuiting.
 * 
 * traverseDij/traverseDijUltra: Dijkstra's implementation for part1/2 respectively.
 * 			A hashtable is used to store visited nodes, and an unordered array stores
 * 			unvisited nodes.
 * 
 * traverseDijUltraHeap: A* optimization(see manhattan()), using a hashtable for visited
 * 			nodes and a min-heap for unvisited nodes, greatly speeding up solution time.
 * 
 * 
 * Note: Each index has many different unique "nodes", because each node is uniquely
 * identified by its row, column, direction, and current direction streak.
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day17(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;

	BLOCK **map = NULL;
	int **imap = NULL;
	int iDim = 0;
	int iParse = 0;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		if (NULL == map)
		{
			iDim = strlen(pCur) - 1; // remove the newline from dimension count
			map = (BLOCK **)malloc(iDim * sizeof(BLOCK *));
			imap = (int **)malloc(iDim * sizeof(int *));
			for (int i = 0; i < iDim; i++)
			{
				map[i] = (BLOCK *)calloc(iDim, sizeof(BLOCK));
				imap[i] = (int *)malloc(iDim * sizeof(int));
			}
		}

		for (int i = 0; i < iDim; i++)
		{
			map[iParse][i].heat = pCur[i] - '0';
			imap[iParse][i] = pCur[i] - '0';
			for (int dir = 0; dir < 4; dir++)
				for (int streak = 0; streak < 3; streak++)
					map[iParse][i].weight[dir][streak] = __INT_MAX__;
			//map[iParse][i].weight = __UINT32_MAX__;
		}
		iParse++;
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	//Start at the top left
	//map[0][0].weight = 0;
	//traverse(map, iDim, 0, 0, 0, RIGHT, -1);
	/*for (int i = 0; i < iDim; i++)
	{
		for (int j = 0; j < iDim; j++)
		{
			printf("%d", imap[i][j]);
		}
		printf("\n");
	}*/
	clock_t before = clock();
	//traverseDij(imap, iDim);
	//traverseDijUltra(imap, iDim);
	traverseDijUltraHeap(imap, iDim);
	clock_t difference = clock() - before;
	int msec = difference * 1000 / CLOCKS_PER_SEC;
	printf("Dij took %d msec\n", msec);
	//todo implement ultratraverse
/*
	int lowest = __INT_MAX__;
	for (int dir = 0; dir < 4; dir++)
		for (int streak = 0; streak < 3; streak++)
		{
			int cur = map[iDim-1][iDim-1].weight[dir][streak];
			if (cur < lowest)
				lowest = cur;
		}
	printf("done, lowest heat loss is %d\n", lowest);
	*/
	//printf("Lowest heat loss to the bottom right is %u\n", map[iDim-1][iDim-1].weight);
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

	day17(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
