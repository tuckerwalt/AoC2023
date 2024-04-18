#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simplehash.h"

//Simple djb2 hash for a string key
unsigned long get_hash(char *key)
{
    unsigned long hash = 5361;
    char c;
    while (c = *key++)
        hash = ((hash << 5) + hash) + c;

    return hash % TABLE_SIZE;
}

/**
 * Adds a new entry to the hash table, using linear probing for collisions
*/
void ht_add(ENTRY **table, char *key, void *value)
{
    static int added = 0;
    unsigned long index = get_hash(key);

    while (table[index] != NULL)
    {
        if (!strcmp(key, table[index]->key))
        {
            printf("Entry %s already found!\n", key);
            return;
        }

        index++;
        if (index >= TABLE_SIZE) 
        {
            index = 0;
        }
    }

    table[index] = (ENTRY *)malloc(sizeof(ENTRY));
    table[index]->key = (char *)malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(table[index]->key, key);
    table[index]->value = value;
    //printf("hashed element %d added\n", ++added);
}

/**
 * Finds an entry in the table using linear probing
 * Returns NULL if not found
 * 
 * Note: infinite loop if the table is full
*/
void *ht_get(ENTRY **table, char *key)
{
    unsigned long index = get_hash(key);
    int bFound = 0;

    while (!bFound)
    {
        if (table[index] == NULL)
            break;
        if (!strcmp(table[index]->key, key))
        {
            return table[index]->value;
        }
        index++;
        if (index >= TABLE_SIZE)
            index = 0;
    }

    return NULL;
}

ENTRY **ht_create()
{
    ENTRY **table = (ENTRY **)malloc(sizeof(ENTRY *) * TABLE_SIZE);
    return table;
}

//Free all memory and wipe the table
void ht_clear(ENTRY **table)
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


