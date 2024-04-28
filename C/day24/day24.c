#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 150
#define MIN_COORD 200000000000000
#define MAX_COORD 400000000000000


typedef struct hail
{
	///position
	long x;
	long y;
	long z;

	///velocity
	int vx;
	int vy;
	int vz;

	/// x-y slope
	long double m;
	/// x-y slope-intercept constant
	long double b;
} HAIL;

//Calculate the slope-intercept form for the hailstone
//y = mx + b
//m and b are stored in the given hailstone
int getSlopeIntercept(HAIL *h)
{
	long double x1 = (long double)h->x;
	long double y1 = (long double)h->y;
	long double z1 = (long double)h->z;
	long double x2 = x1 + (long double)h->vx;
	long double y2 = y1 + (long double)h->vy;
	long double z2 = z1 + (long double)h->vz;

	if (x2 - x1 == (long double)0.0)
	{
		//Undefined slope
		return 1;
	}
	h->m = (y2 - y1) / (x2 - x1);

	h->b = (h->m * ( (long double)0 - x1)) + y1;
	return 0;
	//printf("Line is y = %Lfx + %Lf\n", h->m, h->b);
}

//Print a hailstone to the console
void printHail(HAIL *h, int i)
{
	printf("%d: (%ld, %ld, %ld) - %d, %d, %d\n", i, h->x, h->y, h->z, h->vx, h->vy, h->vz);
}

//Round a floating point number to the nearest whole number
//Used to avoid any weird rounding/calculator errors with decimal numbers
long roundDouble(long double d)
{
	if (d > 0)
		d += 0.5;
	if (d < 0)
		d -= 0.5;
	
	return (long)d;
}

/**
 * Find the intersection point of two hailstones. To be a valid intersection,
 * the hailstones must collide in the future, and they must collide at whole 
 * number coordinates and at a whole number time increment.
 * 
 * Returns 0 if the intersection is valid
*/
int getIntersectionPoint(HAIL *h1, HAIL *h2, long *colx, long *coly, long *colz, int i1, int i2)
{
	if (h1->m == h2->m)
	{
		//printf("hail %d and %d don't collide\n", i1, i2);
		return 1;
	}

	//Simple calculation to find the intersection point of two trajectories 
	//given slope-intercept formulas y = mx + b
	long double cx = (h2->b - h1->b) / (h1->m - h2->m);
	long double cy = ( (h1->m * (h2->b - h1->b)) / (h1->m - h2->m) ) + h1->b;

	if ( (h1->vx < 0 && cx > h1->x) ||
		(h1->vx > 0 && cx < h1->x)  ||
		(h2->vx < 0 && cx > h2->x) ||
		(h2->vx > 0 && cx < h2->x))
	{
		//printf("hail %d and %d collided in the past (%Lf, %Lf)\n", i1, i2, cx, cy);
		return 1;
	}
	else
	{
		//Figure out how many ticks it took to collide
		long double ldTimeDiff = (cx - h1->x) / h1->vx;
		long double decimal = ldTimeDiff - (long double)((long)ldTimeDiff);
		if (decimal > 0.01 && decimal < 0.99)
		{
			//printf("COLLISION at non-whole unit of time!\n");
			return 1;
		}

		long double xdecimal = cx - (long double)((long)cx);
		if (xdecimal > 0.1 && xdecimal < 0.99)
		{
			//printf("Collision x coord is not whole!\n");
			return 1;
		}

		long double cz = (h1->vz * roundDouble(ldTimeDiff)) + h1->z; 
		//printf("hail %d and %d will collide at (%Lf, %Lf, %Lf)\n", i1, i2, cx, cy, cz);
		*colx = roundDouble(cx);
		*coly = roundDouble(cy);
		*colz = roundDouble(cz);
		return 0;
	}
	return 1;
}

/**
 * Finds the intersection of two hailstones. Returns 0(success) if they collide in
 * the future, at the specified (colx, coly, colz), at a whole number tick increment
 * 
 * For optimization, returns 2 if the collision is a failure only due to an 
 * incorrect z coordinate comparison, meaning we are at the correct x-y velocity
*/
int testIntersectionPoint(HAIL *h1, HAIL *h2, long colx, long coly, long colz, int i1, int i2)
{
	if (h1->m == h2->m)
	{
		//printf("hail %d and %d don't collide\n", i1, i2);
		return 1;
	}

	//Simple calculation to find the intersection point of two trajectories 
	//given slope-intercept formulas y = mx + b
	long double cx = (h2->b - h1->b) / (h1->m - h2->m);
	long double cy = ( (h1->m * (h2->b - h1->b)) / (h1->m - h2->m) ) + h1->b;

	if (roundDouble(cx) != colx || roundDouble(cy) != coly)
	{
		//printf("Failed x,y check for hailstone %d\n", i2);
		return 1;
	}

	long double ldTimeDiff = (cx - h2->x) / h2->vx;
	long double decimal = ldTimeDiff - (long double)((long)ldTimeDiff);
	if (decimal > 0.1 && decimal < 0.99)
	{
		//printf("COLLISION at non-whole unit of time!\n");
		return 1;
	}
	long double cz = (h2->vz * roundDouble(ldTimeDiff)) + h2->z; 

	if (roundDouble(cz) != colz)
	{
		//printf("FAILED Z CHECK\n");
		return 2;
	}

	return 0;
}

/**
 * Test a given velocity to see if hailstones can all be hit by a rock
 * thrown from somewhere with that velocity.
 * 
 * Logic:
 * 
 * If every hailstone's velocity is re-framed in terms of the inverse rock velocity
 * and the rock has no velocity, every hailstone should eventually hit the rock at
 * its start position.
*/
int testVelocity(HAIL *hailfield, int iHail, int x, int y, int z)
{
	//Test first two hailstones
	HAIL *h1 = &(hailfield[0]);
	HAIL *h2 = &(hailfield[1]);

	HAIL mh1, mh2;
	mh1.x = h1->x;
	mh1.y = h1->y;
	mh1.z = h1->z;
	mh1.vx = h1->vx - x;
	mh1.vy = h1->vy - y;
	mh1.vz = h1->vz - z;

	mh2.x = h2->x;
	mh2.y = h2->y;
	mh2.z = h2->z;
	mh2.vx = h2->vx - x;
	mh2.vy = h2->vy - y;
	mh2.vz = h2->vz - z;


	long colx = 0;
	long coly = 0;
	long colz = 0;

	if (getSlopeIntercept(&mh1))
	{
		return 1;
	}
	if (getSlopeIntercept(&mh2))
	{
		return 1;
	}
	//printHail(&mh1, 0);
	//printHail(&mh2, 1);

	//If the first two have a valid intersection point at a coordinate,
	//test every other hailstone against that point until failure
	if (!getIntersectionPoint(&mh1, &mh2, &colx, &coly, &colz, 0, 1))
	{
		//printf("Testing velocity %d %d %d\n", x, y, z);
		for (int i = 2; i < iHail; i++)
		{
			HAIL testHail;
			testHail.x = hailfield[i].x;
			testHail.y = hailfield[i].y;
			testHail.z = hailfield[i].z;
			testHail.vx = hailfield[i].vx - x;
			testHail.vy = hailfield[i].vy - y;
			testHail.vz = hailfield[i].vz - z;
			if (getSlopeIntercept(&testHail))
			{
				//just ignore undefined slopes
				continue;
			}

			int ret;
			if (0 != (ret = testIntersectionPoint(&mh1, &testHail, colx, coly, colz, 0, i)))
			{
				//fail
				return ret;
			}
		}
	}
	else
	{
		return 1;
	}

	//found it!!
	printf("valid point (%ld, %ld, %ld) velocity %d %d %d\n", colx, coly, colz, x, y, z);
	printf("total is %ld\n", colx + coly + colz);

	return 0;
}

#define RANGE 300
/**
 * Search for a perfect location and velocity to throw a rock that will hit
 * every hailstone in sequence.
*/
void findIntersectingRock(HAIL *hailfield, int iHail)
{
	int x;
	int y;
	int z = 1;
	int ret;

	/* Accidental discovery: Regardless of what the Z velocity is,
	   If the x and y are correct then hail collision checks will only fail
	   once considering the z position of hailstones, so I only loop through
	   possible x and y until the only error is a z-position mismatch, THEN
	   loop through possible z values
	*/
	for (x = -RANGE; x <= RANGE; x++)
	{
		for (y = -RANGE; y <= RANGE; y++)
		{
			//printf("Testing velocity %d %d %d\n", x, y, z);
			if (2 == (ret = testVelocity(hailfield, iHail, x, y, z)))
			{
				printf("ZCHECK FAILURE AT %d %d\n", x, y);
				goto foundxy;
			}
			else if (ret == 0)
			{
				printf("FOUND\n");
				return;
			}
		}
	}

foundxy:
	for (int z = -RANGE; z <= RANGE; z++)
	{
		//printf("Testing velocity %d %d %d\n", x, y, z);
		if (!testVelocity(hailfield, iHail, x, y, z))
		{
			return;
		}
	}
	return;
}


/**
 * Finds the amount of unique collisions that will occur in the future 
 * within the given coordinate x-y box
*/
int findIntersections(HAIL *hailfield, int iHail, long double dMinCoord, long double dMaxCoord)
{
	int iTotGoodCollisions = 0;
	for (int i1 = 0; i1 < iHail; i1++)
	{
		for (int i2 = i1 + 1; i2 < iHail; i2++)
		{
			HAIL *h1 = &(hailfield[i1]);
			HAIL *h2 = &(hailfield[i2]);

			//If they have the same slope, they won't collide
			if (h1->m == h2->m)
			{
				printf("hail %d and %d don't collide\n", i1, i2);
				continue;
			}

			//Simple calculation to find the intersection point of two trajectories 
			//given slope-intercept formulas y = mx + b
			long double cx = (h2->b - h1->b) / (h1->m - h2->m);
			long double cy = ( (h1->m * (h2->b - h1->b)) / (h1->m - h2->m) ) + h1->b;

			if ( (h1->vx < 0 && cx > h1->x) ||
				(h1->vx > 0 && cx < h1->x)  ||
				(h2->vx < 0 && cx > h2->x) ||
				(h2->vx > 0 && cx < h2->x))
			{
				printf("hail %d and %d collided in the past (%Lf, %Lf)\n", i1, i2, cx, cy);
			}
			else
			{
				if (cx >= dMinCoord && cx <= dMaxCoord &&
					cy >= dMinCoord && cy <= dMaxCoord)
				{
					//printf("hail %d and %d will collide INSIDE at (%Lf, %Lf)\n", i1, i2, cx, cy);
					iTotGoodCollisions++;
				}
				else
				{
					//printf("hail %d and %d will collide outside at (%Lf, %Lf)\n", i1, i2, cx, cy);
				}
			}

		}
	}

	return iTotGoodCollisions;
}

/**
 * You are given a list of hailstones flying through the sky at one point in time:
 * 
 * 19, 13, 30 @ -2,  1, -2
 * 
 * this is a hailstone at position (19, 13, 30) x, y, z 
 *                   with velocity (-2,  1, -2) in each direction.
 * Each tick, the hailstones move that number of cells in their given direction 
 * velocities.
 * 
 * For part one, find the number of unique pairs of hailstones that will collide
 * at some point in the FUTURE in the x-y plane defined by:
 *  200000000000000 <= x <= 400000000000000
 *  200000000000000 <= y <= 400000000000000
 * 
 * 
 * Part two:
 * There exists a scenario in which you throw a new stone from a specific point
 * with a specific velocity such that it collides with every hailstone in the sky.
 * Find the sum of the x, y, and z starting position for this stone.
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day24(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;

	HAIL hailfield[300];
	int iHail = 0;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		HAIL *h = &(hailfield[iHail]);
		sscanf(pCur, "%ld, %ld, %ld @ %d, %d, %d", &h->x, &h->y, &h->z, &h->vx, &h->vy, &h->vz);

		//printHail(h, iHail);
		//getSlopeIntercept(h);
		iHail++;
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	//Part 1
	//int iTot = findIntersections(hailfield, iHail, (long double)MIN_COORD, (long double)MAX_COORD);
	//printf("%d total collisions inside box\n", iTot);

	findIntersectingRock(hailfield, iHail);

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

	day24(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
