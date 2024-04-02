#ifndef SIMPLE_HASH_H
#define SIMPLE_HASH_H

#define TABLE_SIZE 25000

typedef struct sequence
{
    //keys
    char *szSeq;
    short iGroupIndex;
    short bInGroup;
    short iSeqLen;

    //value
    unsigned long lValid;
} SEQUENCE;

void ht_add(SEQUENCE **table, char *szSeq, short iGroupIndex, short bInGroup, short iSeqLen, unsigned long lValid);
SEQUENCE *ht_get(SEQUENCE **table, const char * szSeq, short iGroupIndex, short bInGroup, short iSeqLen);
SEQUENCE **ht_create();
void ht_clear(SEQUENCE **table);

#endif