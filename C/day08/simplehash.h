#ifndef SIMPLE_HASH_H
#define SIMPLE_HASH_H

#define TABLE_SIZE 10000

typedef enum end_type
{
    A,
    Z,
    DEFAULT
} ENDTYPE;

//A node has an id like "AAA" and two connecting nodes, one to the left and right
//If the node ends in 'A', it is type A and similarly is type Z if it ends in 'Z'
typedef struct node
{
    char id[4];
    struct node *left;
    struct node *right;
    //for part 2
    ENDTYPE type;
} NODE;

unsigned long get_hash(const char * str);
NODE *ht_add_or_update(NODE **table, char *key, char *left, char *right);
NODE *ht_get_or_add_empty(NODE **table, const char *key);
NODE *ht_get(NODE **table, const char* key);
NODE **ht_create();

#endif