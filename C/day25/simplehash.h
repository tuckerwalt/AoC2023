#ifndef SIMPLE_HASH_H
#define SIMPLE_HASH_H

#define TABLE_SIZE 20000

//Generic key value
typedef struct entry
{
    char *key;
    void *value;
    int bEmpty;
} ENTRY;

typedef struct table
{
    ENTRY **table;
    int iEntries;
} TABLE;

void ht_add(TABLE *table, char *key, void *value);
void *ht_get(TABLE *table, char *key);
void ht_remove(TABLE *table, char *key);
TABLE *ht_create();
void ht_clear(TABLE *table);
int ht_size(TABLE *table);

#endif