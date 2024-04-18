#ifndef DAY17_H
#define DAY17_H

typedef enum direction
{
	UP,
	DOWN,
	RIGHT,
	LEFT
} DIR;

typedef struct node
{
    //keys
    int row;
    int col;
    DIR direction;
    int sequencelen;

    //value
    int heat;
    int weight;
    int visited;
} NODE;

#endif // DAY17_H