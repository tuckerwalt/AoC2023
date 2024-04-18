#ifndef MINHEAP_H
#define MINHEAP_H

#include "day17.h"

#define MAX_ELEMENTS 100

typedef struct heap_node
{
    int iValue;
    //int iIndex;
    NODE *node;
} HEAPNODE;

typedef struct min_heap
{
    int iMaxElements;
    int iElements;
    HEAPNODE *nodes;
} MINHEAP;

MINHEAP * mh_create();
void mh_insert(MINHEAP *heap, int value, NODE *node);
NODE *mh_pop(MINHEAP *heap);

#endif // MINHEAP_H