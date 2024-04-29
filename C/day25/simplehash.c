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
void ht_add(TABLE *t, char *key, void *value)
{
    static int added = 0;
    unsigned long index = get_hash(key);
    ENTRY **table = t->table;

    int bTombstone = 0;

    while (table[index] != NULL)
    {
        if (table[index]->bEmpty)
        {
            bTombstone = 1;
            break;
        }

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

    if (!bTombstone)
    {
        table[index] = (ENTRY *)malloc(sizeof(ENTRY));
        table[index]->key = (char *)malloc(sizeof(char) * (strlen(key) + 1));
    }
    strcpy(table[index]->key, key);
    table[index]->value = value;
    table[index]->bEmpty = 0;

    (t->iEntries)++;
}

/**
 * Removes an element from the table, leaving a tombstone
*/
void ht_remove(TABLE *t, char *key)
{
    //printf("trying to remove from hash table %s\n", key);
    unsigned long index = get_hash(key);
    ENTRY **table = t->table;

    while (table[index] != NULL)
    {
        if(!table[index]->bEmpty && !strcmp(table[index]->key, key))
        {
            (t->iEntries)--;
            table[index]->bEmpty = 1;
            return;
        }

        index++;
        if (index >= TABLE_SIZE) 
        {
            index = 0;
        }
    }

    printf("HT_REMOVE ERROR %s NOT IN TABLE!\n", key);
}

/**
 * Finds an entry in the table using linear probing
 * Returns NULL if not found
 * 
 * Note: infinite loop if the table is full
*/
void *ht_get(TABLE *t, char *key)
{
    unsigned long index = get_hash(key);
    int bFound = 0;
    ENTRY **table = t->table;

    while (!bFound)
    {
        if (table[index] == NULL)
            break;

        if (!table[index]->bEmpty && !strcmp(table[index]->key, key))
        {
            return table[index]->value;
        }
        index++;
        if (index >= TABLE_SIZE)
            index = 0;
    }

    return NULL;
}

TABLE *ht_create()
{
    TABLE *table = (TABLE *)malloc(sizeof(TABLE));
    table->table = (ENTRY **)malloc(sizeof(ENTRY *) * TABLE_SIZE);
    return table;
}

//Free all memory and wipe the table
void ht_clear(TABLE *t)
{
    ENTRY **table = t->table;
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (NULL != table[i])
        {
            free(table[i]);
            table[i] = NULL;
        }
    }
    t->iEntries = 0;
}

/// Returns the number of entries in the table 
int ht_size(TABLE *t)
{
    return t->iEntries;
}


