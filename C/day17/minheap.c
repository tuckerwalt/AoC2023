#include <stdlib.h>
#include <stdio.h>
#include "day17.h"
#include "minheap.h"

/**
 * Simple min-heap, where the root is the node with the lowest weight
 * and each node is less than its children.
*/
MINHEAP *mh_create()
{
    MINHEAP *mh = (MINHEAP *)malloc(sizeof(MINHEAP));
    mh->iElements = 0;
    mh->iMaxElements = MAX_ELEMENTS;
    mh->nodes = (HEAPNODE *)malloc(MAX_ELEMENTS * sizeof(HEAPNODE));

    return mh;
}

void printheap(MINHEAP *heap)
{
    printf("%d elements: ", heap->iElements);
    for (int i = 0; i < heap->iElements; i++)
    {
        printf("%d ", heap->nodes[i].iValue);
    }
    printf("\n");
}

int mh_peek(MINHEAP *heap)
{
    return heap->nodes[0].iValue;
} 

//parent node (i - 1) / 2
//left child (i * 2) + 1
//right child (i * 2) + 2

//Swap two values in the heap
void mh_swap (MINHEAP *heap, int i1, int i2)
{
    int valuesav = heap->nodes[i1].iValue;
    NODE *nodesav = heap->nodes[i1].node;

    heap->nodes[i1].iValue = heap->nodes[i2].iValue;
    heap->nodes[i1].node = heap->nodes[i2].node;

    heap->nodes[i2].iValue = valuesav;
    heap->nodes[i2].node = nodesav;
}

//Fix the heap structure going up the tree, starting from a given node
void mh_bubbleUp(MINHEAP *heap, int index)
{
    int i = index;
    while (0 != i && 
            heap->nodes[(i-1)/2].iValue > heap->nodes[i].iValue)
    {
        mh_swap(heap, i, (i-1)/2);
        i = (i-1)/2;
    }
}

//Start at the root and validate the heap all the way down
void mh_heapify(MINHEAP *heap)
{
    int i = 0;
    int iNodes = heap->iElements;

    while (i < iNodes)
    {
        int left = (2*i)+1;
        int right = (2*i)+2;
        int swapIndex = i;

        if (left < iNodes && heap->nodes[left].iValue < heap->nodes[i].iValue)
            swapIndex = left;
        if (right < iNodes && 
                heap->nodes[right].iValue < heap->nodes[i].iValue &&
                heap->nodes[right].iValue < heap->nodes[left].iValue)
            swapIndex = right;

        if (swapIndex != i)
        {
            mh_swap(heap, i, swapIndex);
            i = swapIndex;
        }
        else 
            break;

    }
}

//Insert a new node at the correct location
void mh_insert(MINHEAP *heap, int value, NODE *node)
{
    if (heap->iElements == heap->iMaxElements)
    {
        heap->iMaxElements *= 2;
        heap->nodes = (HEAPNODE *)realloc(heap->nodes, heap->iMaxElements * sizeof(HEAPNODE));
    }

    int index = heap->iElements;

    heap->nodes[index].iValue = value;
    //heap->nodes[index].iIndex = index;
    heap->nodes[index].node = node;
    heap->iElements++;

    mh_bubbleUp(heap, index);

    //printf("Inserted (%d, %d), value %d\n", node->row, node->col, value);
    //printf("Top of heap is %d\n", mh_peek(heap));
    //printheap(heap);

}

//Remove the root node(The one with the lowest weight)
NODE *mh_pop(MINHEAP *heap)
{
    //printf("popping top of heap, currently %d elements\n", heap->iElements);
    if (heap->iElements < 1)
        return NULL;

    NODE *ret = heap->nodes[0].node;

    if (heap->iElements > 1)
    {
        //int lastIndex = heap->iElements - 1;
        mh_swap(heap, 0, heap->iElements - 1);
    }
    
    heap->iElements--;
    mh_heapify(heap);

    return ret;
}
