#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 150

typedef enum direction
{
	UP,
	DOWN,
	RIGHT,
	LEFT
} DIR;

typedef struct charges
{
	int chargedirs[4];
} CHARGE;

//forward declaration
void passThroughCell(char **mirrors, CHARGE **energy, int iDim, int row, int col, DIR dir);

//Given a current position and a direction, return the next coordinates for the beam
void getNextPos(DIR dir, int row, int col, int *newrow, int *newcol)
{
	switch(dir)
	{
		case UP:
			*newrow = row - 1;
			*newcol = col;
			break;
		case DOWN:
			*newrow = row + 1;
			*newcol = col;
			break;
		case RIGHT:
			*newrow = row;
			*newcol = col + 1;
			break;
		case LEFT:
			*newrow = row;
			*newcol = col - 1;
			break;
		default:
			printf("Unexpected direction %d\n", dir);
			break;
	}
}

//Finds the next coordinates for the beam given its direction and continues
void moveBeam(char **mirrors, CHARGE **energy, int iDim, int row, int col, DIR dir)
{
	int nextRow = 0;
	int nextCol = 0;
	getNextPos(dir, row, col, &nextRow, &nextCol);
	passThroughCell(mirrors, energy, iDim, nextRow, nextCol, dir);
}

/**
 * Recursively shine a beam around the tile map.
 * 'dim' is the direction that the beam is currently travelling in.
*/
void passThroughCell(char **mirrors, CHARGE **energy, int iDim, int row, int col, DIR dir)
{
	//Don't go outside of the grid
	if ((row < 0 || row >= iDim) ||
		(col < 0 || col >= iDim))
	{
		return;
	}

	//Memoization; If this tile has been charged by another beam from the same
	//direction, break out of the loop
	if (energy[row][col].chargedirs[dir])
		return;

	//energize cell
	energy[row][col].chargedirs[dir]++;

	char cur = mirrors[row][col];
	//Ugly, but switch statements are nice and fast
	switch(cur)
	{
		case '.':
		//Reflect
			moveBeam(mirrors, energy, iDim, row, col, dir);
			break;
		case '/':
		//Reflect
			switch(dir)
			{
				case UP:
					moveBeam(mirrors, energy, iDim, row, col, RIGHT);
					break;
				case DOWN:
					moveBeam(mirrors, energy, iDim, row, col, LEFT);
					break;
				case LEFT:
					moveBeam(mirrors, energy, iDim, row, col, DOWN);
					break;
				case RIGHT:
					moveBeam(mirrors, energy, iDim, row, col, UP);
					break;
			}
			break;
		case '\\':
		//Reflect
			switch(dir)
			{
				case UP:
					moveBeam(mirrors, energy, iDim, row, col, LEFT);
					break;
				case DOWN:
					moveBeam(mirrors, energy, iDim, row, col, RIGHT);
					break;
				case LEFT:
					moveBeam(mirrors, energy, iDim, row, col, UP);
					break;
				case RIGHT:
					moveBeam(mirrors, energy, iDim, row, col, DOWN);
					break;
			}
			break;
		case '|':
			switch(dir)
			{
				case UP:
				case DOWN:
					moveBeam(mirrors, energy, iDim, row, col, dir);
					break;
				case LEFT:
				case RIGHT:
					//Split the beam
					moveBeam(mirrors, energy, iDim, row, col, UP);
					moveBeam(mirrors, energy, iDim, row, col, DOWN);
					break;
			}
			break;
		case '-':
			switch(dir)
			{
				case LEFT:
				case RIGHT:
					moveBeam(mirrors, energy, iDim, row, col, dir);
					break;
				case UP:
				case DOWN:
					//Split the beam
					moveBeam(mirrors, energy, iDim, row, col, LEFT);
					moveBeam(mirrors, energy, iDim, row, col, RIGHT);
					break;
			}
			break;
		default:
			printf("Unexpected char in map: %c\n", cur);
			break;
	}

}

/**
 * Returns a positive number if the tile has at least one beam charge
*/
int countCharge(CHARGE charge)
{
	return charge.chargedirs[0] +
		charge.chargedirs[1] +
		charge.chargedirs[2] +
		charge.chargedirs[3];
}

/**
 * Print the charge map to console
*/
void printEnergyMap(CHARGE **energy, int iDim)
{
	for (int i = 0; i < iDim; i++)
	{
		for (int j = 0; j < iDim; j++)
		{
			int charge = countCharge(energy[i][j]);
			if (charge)
				printf("%d", charge);
			else
				printf("%c", '.');
		}
		printf("\n");
	}
}

/**
 * Print the map to console
*/
void printMirrors(char **mirrors, int iDim)
{
	for (int i = 0; i < iDim; i++)
	{
		for (int j = 0; j < iDim; j++)
		{
			printf("%c", mirrors[i][j]);
		}
		printf("\n");
	}
}

/**
 * Given an energy map, counts the number of cells with at least one charge
*/
int countCharges(CHARGE **energy, int iDim)
{
	int iTotCharge = 0;
	for (int i = 0; i < iDim; i++)
	{
		for (int j = 0; j < iDim; j++)
		{
			int charge = countCharge(energy[i][j]);
			if (charge)
				iTotCharge++;
		}
	}
	return iTotCharge;
}

/**
 * Passes a light through the map at a specific entry point and direction, and counts
 * the number of charged cells in the map.
*/
int testBeam(char **mirrormap, CHARGE **energymap, int iDim, int row, int col, DIR dir)
{
	//clean the energy map
	for (int i = 0; i < iDim; i++)
	{
		memset(energymap[i], 0, sizeof(CHARGE) * iDim);
	}

	passThroughCell(mirrormap, energymap, iDim, row, col, dir);
	return countCharges(energymap, iDim);
}

/**
 * You are given a 2-d map of mirrors and light splitters:
 * 
 * .|...\....
 * |.-.\.....
 * .....|-...
 * ........|.
 * ..........
 * .........\
 * ..../.\\..
 * .-.-/..|..
 * .|....-|.\
 * ..//.|....
 * 
 * A light beam can enter from any edge of the map, in any cardinal direction.
 * 
 *    '.' is empty space allowing the beam to pass through.
 *    '-' and '|' are beam splitters that split the light if the beam is perpendicular
 *         to the splitter.
 *    '/' and '\' are mirrors that reflect the light. For example, if the beam is 
 *         travelling upwards and hits a '/', the beam will continue to the right.
 * 
 * Each location that a beam passes through becomes charged with energy. For example,
 * if a beam of light enters the top left corner travelling to the right, these 46 
 * tiles would be charged:
 * 
 * ######....
 * .#...#....
 * .#...#####
 * .#...##...
 * .#...##...
 * .#...##...
 * .#..####..
 * ########..
 * .#######..
 * .#...#.#..
 * 
 * Find the entry point for a light beam that results in the highest amount of charged
 * tiles.
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day16(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;

	//The map of tiles with mirrors/splitters
	char **mirrormap = NULL;
	//The map of charged tiles. Each tile records the directions that a beam
	//has travelled through it previously, to avoid loops.
	CHARGE **energymap = NULL;
	
	int iDim = 0;
	int iParse = 0;

	int iMaxCharge = 0;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		if (NULL == mirrormap)
		{
			iDim = strlen(pCur) - 1; // remove the newline from dimension count
			mirrormap = (char **)malloc(iDim * sizeof(char *));
			energymap = (CHARGE **)malloc(iDim * sizeof(CHARGE *));
			for (int i = 0; i < iDim; i++)
			{
				mirrormap[i] = (char *)malloc((iDim + 2) * sizeof(char));
				energymap[i] = (CHARGE *)calloc(iDim, sizeof(CHARGE));
			}
		}
		strcpy(mirrormap[iParse++], pCur);
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	//For each entry point around the perimeter of the map, check how many cells
	//get charged
	for (int i = 0; i < iDim; i++)
	{
		int charge = 0;

		charge = testBeam(mirrormap, energymap, iDim, i, 0, RIGHT);
		if (charge > iMaxCharge)
			iMaxCharge = charge;

		charge = testBeam(mirrormap, energymap, iDim, i, iDim - 1, LEFT);
		if (charge > iMaxCharge)
			iMaxCharge = charge;
			
		charge = testBeam(mirrormap, energymap, iDim, 0, i, DOWN);
		if (charge > iMaxCharge)
			iMaxCharge = charge;

		charge = testBeam(mirrormap, energymap, iDim, iDim - 1, i, UP);
		if (charge > iMaxCharge)
			iMaxCharge = charge;
	}

	printf("done, max charge is %d\n", iMaxCharge);
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

	day16(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
