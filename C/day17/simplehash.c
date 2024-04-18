#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simplehash.h"

/**
 * Compare key values to a node entry.
 * Returns:
 *     0 if the keys perfectly match the given node
 *     1 otherwise
*/
int nodecmp(NODE *node, int row, int col, DIR direction, int sequencelen)
{
    if (row == node->row &&
        col == node->col &&
        direction == node->direction &&
        sequencelen == node->sequencelen)
    {
        return 0;
    }
    return 1;
}

//Simple djb2 hash for a multi-variable key
unsigned long get_hash(int row, int col, DIR direction, int sequencelen)
{
    unsigned long hash = 5361;

    hash = ((hash << 5) + hash) + row;
    hash = ((hash << 5) + hash) + col;
    hash = ((hash << 5) + hash) + direction;
    hash = ((hash << 5) + hash) + sequencelen;

    return hash % TABLE_SIZE;
}

/**
 * Adds a new sequence entry to the hash table, using linear probing for collisions
*/
NODE *ht_add(NODE **table, int row, int col, DIR direction, int sequencelen, int heat, int weight, int visited)
{
    static int added = 0;
    unsigned long index = get_hash(row, col, direction, sequencelen);
    //printf("adding new %d %d %d %d\n", row, col, direction, sequencelen);
    //Allocate a new table entry with the given keys/value
    NODE *newnode = (NODE *)malloc(sizeof(NODE));
    //newnode->szSeq = (char *)malloc((sizeof(char) * strlen(szSeq)) + 1);
    //strcpy(newnode->szSeq, szSeq);
    
    newnode->row = row;
    newnode->col = col;
    newnode->direction = direction;
    newnode->sequencelen = sequencelen;
    newnode->heat = heat;
    newnode->weight = weight;
    newnode->visited = visited;

    while (table[index] != NULL)
    {
        index++;
        if (index >= TABLE_SIZE) 
        {
            index = 0;
        }
    }

    table[index] = newnode;
    //printf("hashed element %d added\n", ++added);
    return newnode;
}

/**
 * Finds an entry in the table using linear probing
 * Returns NULL if not found
 * 
 * Note: infinite loop if the table is full
*/
NODE *ht_get(NODE **table, int row, int col, DIR direction, int sequencelen)
{
    unsigned long index = get_hash(row, col, direction, sequencelen);
    int bFound = 0;

    //printf("looking for hash element %d %d %d %d\n", row, col, direction, sequencelen);
    while (!bFound)
    {
        if (table[index] == NULL)
            break;
        if (!nodecmp(table[index], row, col, direction, sequencelen))
        {
            //printf("FOUND %s!\n", szSeq);
            return table[index];
        }
        index++;
        if (index >= TABLE_SIZE)
            index = 0;
    }

    return NULL;
}

NODE **ht_create()
{
    NODE **table = (NODE **)malloc(sizeof(NODE *) * TABLE_SIZE);
    return table;
}

//Free all memory and wipe the table
void ht_clear(NODE **table)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (NULL != table[i])
        {
            free(table[i]);
            table[i] = NULL;
        }
    }
}


