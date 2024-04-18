#ifndef SIMPLE_HASH_H
#define SIMPLE_HASH_H

#define TABLE_SIZE 20000

//Generic key value
typedef struct entry
{
    char *key;
    void *value;
} ENTRY;

void ht_add(ENTRY **table, char *key, void *value);
void *ht_get(ENTRY **table, char *key);
ENTRY **ht_create();
void ht_clear(ENTRY **table);

#endif