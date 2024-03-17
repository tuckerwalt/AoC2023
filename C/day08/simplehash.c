#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simplehash.h"

//Simple djb2 hash
unsigned long get_hash(const char * str)
{
    unsigned long hash = 5361;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;

    return hash % TABLE_SIZE;
}

/**
 * Adds a new Node to the hash table, or updates the connecting nodes if the 
 * node exists.
 * Returns 0 on success.
*/
NODE *ht_add_or_update(NODE **table, char *key, char *left, char *right)
{
    unsigned long index = get_hash(key);

    while (table[index] != NULL && strcmp(table[index]->id, key))
    {
        index++;
        if (index >= TABLE_SIZE) 
        {
            index = 0;
        }
    }

    //If the node hasn't been allocated, malloc it 
    if (table[index] == NULL)
    {
        table[index] = (NODE *)malloc(sizeof(NODE));
        if (NULL == table[index])
        {
            printf("MEMORY ERROR\n");
            return NULL;
        }

        //Set the node type, used in part 2
        strcpy(table[index]->id, key);
        if (key[2] == 'A')
            table[index]->type = A;
        else if (key[2] == 'Z')
            table[index]->type = Z;
        else   
            table[index]->type = DEFAULT;
    }

    table[index]->left = ht_get_or_add_empty(table, left);
    table[index]->right = ht_get_or_add_empty(table, right);

    return table[index];
}

//Note: infinite loop if the table is full
NODE *ht_get_or_add_empty(NODE **table, const char *key)
{
    unsigned long index = get_hash(key);
    int bFound = 0;

    while (!bFound)
    {
        if (table[index] == NULL)
            break;
        if (!strcmp(table[index]->id, key))
            return table[index];

        index++;
        if (index >= TABLE_SIZE)
            index = 0;
    }

    //If we didn't find it, allocate a new Node
    table[index] = (NODE *)malloc(sizeof(NODE));
    strcpy(table[index]->id, key);

    //Set the node type, used in part 2
    if (key[2] == 'A')
        table[index]->type = A;
    else if (key[2] == 'Z')
        table[index]->type = Z;
    else   
        table[index]->type = DEFAULT;

    return table[index];
}

//Note: infinite loop if the table is full
NODE *ht_get(NODE **table, const char* key)
{
    unsigned long index = get_hash(key);
    int bFound = 0;

    while (!bFound)
    {
        if (table[index] == NULL)
            break;
        if (!strcmp(table[index]->id, key))
            return table[index];

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
