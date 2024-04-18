#ifndef SIMPLE_HASH_H
#define SIMPLE_HASH_H
#include "day17.h"

#define TABLE_SIZE 1000000

NODE *ht_add(NODE **table, int row, int col, DIR direction, int sequencelen, int heat, int weight, int visited);
NODE *ht_get(NODE **table, int row, int col, DIR direction, int sequencelen);
NODE **ht_create();
void ht_clear(NODE **table);

#endif