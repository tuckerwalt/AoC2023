#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 20

//hardcoded starting indexes to guarantee that we don't go negative
#define START_X 200
#define START_Y 130
#define START_P2X 2800000
#define START_P2Y 1000000

#define START_DIM 200

typedef enum direction
{
	RIGHT,
	DOWN,
	LEFT,
	UP
} DIR;

#define DIRUP      1 << UP
#define DIRDOWN    1 << DOWN
#define DIRLEFT    1 << LEFT
#define DIRRIGHT   1 << RIGHT
#define UPRIGHT    DIRUP | DIRRIGHT
#define UPLEFT     DIRUP | DIRLEFT
#define DOWNRIGHT  DIRDOWN | DIRRIGHT
#define DOWNLEFT   DIRDOWN | DIRLEFT

static int dirs[4][2] = 
{
	{0, 1},
	{1, 0},
	{0, -1},
	{-1, 0}
};

//Basic shoelace formula for calculating area of a polygon
long shoelace(long *xCoords, long *yCoords, int iCoords)
{
	long total = 0;

	for (int i = 1; i < iCoords; i++)
	{
		total += ((xCoords[i] * yCoords[i - 1]) -
					(xCoords[i - 1] * yCoords[i]));
	}

	return total / 2;
}

/**
 * Given a list of directions that dig a trench within a 2-d grid:
 * R 6 (#70c710)
 * D 5 (#0dc571)
 * L 2 (#5713f0)
 * 
 * For part one, these would mean: 
 * dig right for 6 tiles
 * dig down for 5 tiles
 * dig left for 2 tiles
 * 
 * The directions will always result in a perfect loop.
 * Find the total size of the dug trench, including the tiles contained within.
 * 
 * For part two the 6 digit hexadecimal defines new directions:
 * the first five digits(70c71) are a hexadecimal encoding of the actual 
 * length to dig: 461937
 * the sixth digit is the direction to dig: 0:right 1:down 2:left 3:up
*/
void day18shoe(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;

	long x = START_X;
	long y = START_Y;
	long p2x = START_P2X;
	long p2y = START_P2Y;

	long xcoords[1000];
	long ycoords[1000];
	long p2xcoords[1000];
	long p2ycoords[1000];
	xcoords[0] = x;
	ycoords[0] = y;

	p2xcoords[0] = p2x;
	p2ycoords[0] = p2y;

	long minY = 10000000000;
	long minX = 10000000000;

	int iCoords = 1;
	long lBorder = 0;
	long p2lBorder = 0;

	char direction;
	long lTiles;
	char hex[7] = {0};
	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		if (EOF == sscanf(pCur, "%c %ld (#%6s)", &direction, &lTiles, hex))
			printf("scan error %s\n", strerror(errno));


		long xdiff = (long)dirs[dir(direction)][0];
		long ydiff = (long)dirs[dir(direction)][1];
		x += lTiles * xdiff;
		y += lTiles * ydiff;
		xcoords[iCoords] = x;
		ycoords[iCoords] = y;

		if (x < minX)
			minX = x;
		if (y < minY)
			minY = y;

		DIR p2dir = hex[5] - '0';
		hex[5] = '\0';
		int p2hex = 0;
		sscanf(hex, "%x", &p2hex);
		//printf("%d %d\n", p2dir, p2hex);
		long p2xdiff = (long)dirs[p2dir][0];
		long p2ydiff = (long)dirs[p2dir][1];
		p2x += (long)p2hex * p2xdiff;
		p2y += (long)p2hex * p2ydiff;

		p2xcoords[iCoords] = p2x;
		p2ycoords[iCoords] = p2y;

		if (p2x < minX)
			minX = p2x;
		if (p2y < minY)
			minY = p2y;

		iCoords++;

		p2lBorder += p2hex;
		lBorder += lTiles;

	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}
	
	if (minX < 0 || minY < 0)
		return;
	long lTot = shoelace(xcoords, ycoords, iCoords) + ((lBorder + 2) / 2);
	long p2lTot = shoelace(p2xcoords, p2ycoords, iCoords) + ((p2lBorder + 2) / 2);
	printf("Area of trench is %ld, size of border is %ld, number of coords is %d\n", shoelace(xcoords, ycoords, iCoords), lBorder, iCoords - 1);
	printf("Total trench area is %ld\n", lTot);
	printf("Part 2 area of trench is %ld, size of border is %ld, number of coords is %d\n", shoelace(p2xcoords, p2ycoords, iCoords), p2lBorder, iCoords - 1);
	printf("Part 2 total trench area is %ld\n", p2lTot);
}

int main(int argc, char **argv)
{
	char *szInputFile = NULL;
	FILE *pFile = NULL;
	int ret = 0;

	szInputFile = (char *)malloc(256 * sizeof(char));
	if (NULL == szInputFile)
	{
		printf("Failed to allocate memory\n");
		return -1;
	}

	strcpy(szInputFile, argc > 1 ? argv[1] : INPUTNAME);

	pFile = fopen(szInputFile, "r");
	if (NULL == pFile)
	{
		printf("Failed to open %s\n", szInputFile);
		return -1;
	}

	//day18(pFile);
	day18shoe(pFile);
	
	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
