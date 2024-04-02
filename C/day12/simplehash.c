#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simplehash.h"

/**
 * Compare key values to a sequence entry.
 * Returns:
 *     0 if the keys perfectly match the given sequence
 *     1 otherwise
*/
int seqcmp(SEQUENCE *seq, char *szSeq, short iGroupIndex, short bInGroup, short iSeqLen)
{
    if (iGroupIndex != seq->iGroupIndex)
        return 1;

    if (bInGroup != seq->bInGroup)
        return 1;

    if (iSeqLen != seq->iSeqLen)
        return 1;

    if (strcmp(seq->szSeq, szSeq))
        return 1;

    return 0;
}

//Simple djb2 hash for a multi-variable key
unsigned long get_hash(const char * seq, short iGroupIndex, short bInGroup, short iSeqLen)
{
    unsigned long hash = 5361;
    int c;

    while (c = *seq++)
        hash = ((hash << 5) + hash) + c;

    hash = ((hash << 5) + hash) + iGroupIndex;
    hash = ((hash << 5) + hash) + bInGroup;
    hash = ((hash << 5) + hash) + iSeqLen;

    return hash % TABLE_SIZE;
}

/**
 * Adds a new sequence entry to the hash table, using linear probing for collisions
*/
void ht_add(SEQUENCE **table, char *szSeq, short iGroupIndex, short bInGroup, short iSeqLen, unsigned long lValid)
{
    unsigned long index = get_hash(szSeq, iGroupIndex, bInGroup, iSeqLen);

    //Allocate a new table entry with the given keys/value
    SEQUENCE *newseq = (SEQUENCE *)malloc(sizeof(SEQUENCE));
    newseq->szSeq = (char *)malloc((sizeof(char) * strlen(szSeq)) + 1);
    strcpy(newseq->szSeq, szSeq);
    newseq->iGroupIndex = iGroupIndex;
    newseq->bInGroup = bInGroup;
    newseq->iSeqLen = iSeqLen;
    newseq->lValid = lValid;


    while (table[index] != NULL)
    {
        index++;
        if (index >= TABLE_SIZE) 
        {
            index = 0;
        }
    }

    table[index] = newseq;
}

/**
 * Finds an entry in the table using linear probing
 * Returns NULL if not found
 * 
 * Note: infinite loop if the table is full
*/
SEQUENCE *ht_get(SEQUENCE **table, const char * szSeq, short iGroupIndex, short bInGroup, short iSeqLen)
{
    unsigned long index = get_hash(szSeq, iGroupIndex, bInGroup, iSeqLen);
    int bFound = 0;

    while (!bFound)
    {
        if (table[index] == NULL)
            break;
        if (!seqcmp(table[index], szSeq, iGroupIndex, bInGroup, iSeqLen))
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

SEQUENCE **ht_create()
{
    SEQUENCE **table = (SEQUENCE **)malloc(sizeof(SEQUENCE *) * TABLE_SIZE);
    return table;
}

//Free all memory and wipe the table
void ht_clear(SEQUENCE **table)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (NULL != table[i])
        {
            free(table[i]->szSeq);
            free(table[i]);
            table[i] = NULL;
        }
    }
}


