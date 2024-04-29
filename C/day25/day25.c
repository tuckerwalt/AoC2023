#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include "simplehash.h"
#include <time.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 100
#define BASE_CAPACITY 10

/** 
 * A connection simply holds a pointer to an edge
 * Connections are unique, but edges may be shared
 * For example, when node A connects to node B, both nodes hold a unique 
 * connection structure for this edge in their list, but both connections 
 * point to the same edge in memory. 
*/ 
typedef struct connection CONNECTION;

/**
 * A node in an undirected graph, storing a count of merged nodes and
 * an adjacency list for connected nodes.
*/
typedef struct node
{
	/// Node ID
	char name[4];

	/// Doubly linked list of connections(edges) for this node
	CONNECTION *connections;
	
	/// A count of the nodes that have been merged into this node(+1)
	int iNodesInNode;
} NODE;

/**
 * An edge, connecting two nodes.
 * Each edge stores its index into the edge array for fast access
*/
typedef struct edge
{
	NODE *n1;
	NODE *n2;
	int index;
} EDGE;

struct connection
{
	EDGE *edge;

	/// linked list
	struct connection *prev;
	struct connection *next;
};

/// Prints the edge array
void printEdges(EDGE **edges, int iEdges)
{
	printf("%d edges:\n", iEdges);
	for (int i = 0; i < iEdges; i++)
	{
		printf("\t EDGE %d: %p: (%s:%d, %s:%d)\n", i, edges[i], edges[i]->n1->name, edges[i]->n1->iNodesInNode, edges[i]->n2->name, edges[i]->n2->iNodesInNode);
	}
}

/// Add a connection to the front of a node's adjacency list
void addConnection(NODE *n, CONNECTION *c)
{
	c->prev = NULL;
	c->next = n->connections;
	if (c->next != NULL)
		c->next->prev = c;
	n->connections = c;
}

/**
 * Given an edge, finds the relevant connection in the target node and frees it
*/
void removeConnectionViaEdge(NODE *n, EDGE *e)
{
	CONNECTION *c = n->connections;
	while (c != NULL)
	{
		if (c->edge == e)
		{
			//printf("removing %p from %s, (%s, %s)\n", c, n->name, c->edge->n1->name, c->edge->n2->name);
			if (c->prev == NULL)
			{
				n->connections = c->next;
				if (NULL != n->connections)
					n->connections->prev = NULL;
				break;
			}
			else
			{
				c->prev->next = c->next;
				if (c->next != NULL)
					c->next->prev = c->prev;
				break;
			}
		}
		c = c->next;
	}

	if (c == NULL)
	{
		printf("removeConnectionViaEdge: EDGE NOT FOUND!\n");
		exit(1);
	}

	//printf("freeing %p\n", c);
	free(c);
}

/**
 * Removes an edge from the edge array by moving the edge at the end of the 
 * array into its position, then freeing it.
 * 
 * @param iEdges The size of the array
*/
void findAndFreeEdge(EDGE **edges, int *iEdges, EDGE *edge)
{
	int i = edge->index;
	//Take the last element and place it in our spot
	edges[i] = edges[(*iEdges) - 1];
	edges[i]->index = i;
	(*iEdges)--;
	free(edge);
}

/**
 * Merges two nodes in the graph by:
 * - Selecting a node to be consumed into the other
 * - Removing this edge from both nodes
 * - Moving all edges from the consumed node to the host node, reframing them in
 * terms of the host. This may result in a self-referential edge, in which case
 * we just remove it.
*/
void mergeEdge(EDGE *edge, EDGE **edges, int *iEdges)
{
	/// n1 is the host node
	NODE *n1 = edge->n1;
	/// n2 is the donor that will be consumed
	NODE *n2 = edge->n2;
	/// The current connection being evaluated for removal/moving
	CONNECTION *con = NULL;
	
	// First, remove this edge from the host
	con = n1->connections;
	while(NULL != con)
	{
		CONNECTION *next = con->next;
		// If this connection is referring to the edge we want to delete, remove it
		if (con->edge == edge)
		{
			//printf("removing %p from %s, (%s, %s)\n", con, n1->name, con->edge->n1->name, con->edge->n2->name);
			if (con->prev == NULL)
			{
				n1->connections = con->next;
				if (NULL != n1->connections)
					n1->connections->prev = NULL;
				break;
			}
			else
			{
				con->prev->next = con->next;
				if (con->next != NULL)
					con->next->prev = con->prev;
				break;
			}
		}

		// Move to the next connection in the list
		con = next;
	}
	if (NULL == con)
	{
		printf("Fatal: didn't find connection to remove\n");
		exit(1);
	}
	//printf("freeing %p\n", con);
	free(con);

	// Next, remove this edge from the donor, and move all other connections into the host
	con = n2->connections;
	while(NULL != con)
	{
		CONNECTION *next = con->next;
		if (con->edge == edge)
		{
			//For the target edge, simply remove the connection and free it
			//printf("removing %p from %s, (%s, %s)\n", con, n2->name, con->edge->n1->name, con->edge->n2->name);
			if (con->prev == NULL)
			{
				n2->connections = con->next;
				if (NULL != n2->connections)
					n2->connections->prev = NULL;
			}
			else
			{
				con->prev->next = con->next;
				if (con->next != NULL)
					con->next->prev = con->prev;
			}

			//printf("freeing %p\n", con);
			free(con);
		}
		else
		{
			/*
		      For every other connection, add it to the first node
			  Example:
			    We are merging B into A, and this current edge is from
			    C to B. Change the edge to connect C to A and add it to 
				A. Since C holds a reference to this same edge, we don't need
				to do anything on C's side.
			*/
			if (con->edge->n1 == n2)
			{
				con->edge->n1 = n1;
			}
			else if (con->edge->n2 == n2)
			{
				con->edge->n2 = n1;
			}
			else
			{
				printf("Fatal, did not find ourselves in this connection\n");
				exit(1);
			}

			/*
			  As we merge nodes, some of these edges will end up pointing a node
			  to itself. If this happens, simply remove/free the connection, and 
			  remove the edge from the list of all edges.
			*/
			if (con->edge->n2 == con->edge->n1)
			{
				removeConnectionViaEdge(n1, con->edge);
				//printf("freeing %p\n", con);
				findAndFreeEdge(edges, iEdges, con->edge);
				free(con);
			}
			else
			{
				// Not self-referential, so add it to the host node
				addConnection(n1, con);
			}

			// Remove the connection from the donor
			n2->connections = next;
			if (NULL != n2->connections)
				n2->connections->prev = NULL;
		}

		// Move on to the next connection
		con = next;
	}

	// Add the # of nodes inside the donor to the host 
	n1->iNodesInNode += n2->iNodesInNode;
}

/**
 * Karger's method for finding a minimum cut in an undirected graph.
 * It's very fast but random, with a very low chance of finding the actual
 * lowest cut.
 * 
 * Algorithm:
 * - Pick a random edge in the graph
 * - Merge the nodes from that edge together, combining their edges
 * - Remove any resulting circular edges as a result of merging
 * - Repeat until there are only two nodes left
 * 
 * Note: This is destructive and modifies the actual graph passed in.
*/
long findPossibleMinimumCut(TABLE *table, EDGE **edges, int *iEdges)
{
	//printEdges(edges, *iEdges);
	// Repeat until there are only two nodes left.
	while (ht_size(table) > 2)
	{
		// Pick a random edge
		int index = rand() % (*iEdges);
		EDGE *edge = edges[index];

		//printf("pulling edge %d, (%s, %s)\n", index, edge->n1->name, edge->n2->name);

		// Merge the nodes (node 2 into node 1)
		mergeEdge(edge, edges, iEdges);
		
		// Remove node 2 from the graph entirely and free the memory
		ht_remove(table, edge->n2->name);
		free(edge->n2);

		// Finally remove the edge from the edges array and free it
		int newI = edge->index;
		edges[newI] = edges[(*iEdges) - 1];
		edges[newI]->index = newI;
		(*iEdges)--;

		//printf("removed edge, %d edges remain\n", *iEdges);
		free(edge);

		//printEdges(edges, *iEdges);
	}

	// The target in this puzzle is a minimum cut of three, so print out the two
	// group sizes from that cut.
	// We can take any of the edges from the list because they will all refer to the 
	// same two nodes at this point.
	EDGE *e = edges[0];
	if (*iEdges == 3)
		printf("node 1 size %d, node 2 size %d\n", e->n1->iNodesInNode, e->n2->iNodesInNode);
	return (long)e->n1->iNodesInNode * (long)e->n2->iNodesInNode;
}

/**
 * Add an edge connecting two given nodes in the graph.
 * Allocates a new edge, adds it to two new connections, and adds the
 * edge to each node's adjacency list via the allocated connections.
*/
void connectNodes(NODE *n1, NODE *n2, EDGE **edges, int *iEdges)
{
	CONNECTION *con1 = (CONNECTION *)malloc(sizeof(CONNECTION)); 
	CONNECTION *con2 = (CONNECTION *)malloc(sizeof(CONNECTION));
	EDGE *newedge = (EDGE *)malloc(sizeof(EDGE ));

	// Add the edge to the edge array
	edges[*iEdges] = newedge;
	newedge->n1 = n1;
	newedge->n2 = n2;
	newedge->index = *iEdges;
	*iEdges += 1;

	// Add each connection to the respective node
	con1->edge = newedge;
	con2->edge = newedge;
	addConnection(n1, con1);
	addConnection(n2, con2);
}

/**
 * Given a description for an undirected graph:
 *   rsh: frs pzl lsr  - (node rsh connects to frs, pzl, and lsr)
 *   xhk: hfx - (node xhk connects to hfx)
 * 
 * Find the product of the size of the two groups resulting from the minimum cut
 * on the input graph. The minimum cut is already told to be 3.
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day25(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;

	// Random seed
	srand(time(NULL));
	
	// Undirected Graph structures:
	// Hash table to store nodes during edge creation and to count nodes
	TABLE *table = ht_create();
	// Unsorted array of graph edges
	EDGE *edges[5000];
	int iEdges = 0;

	// For each iteration:
	// Rather than going through the hassle of duplicating nodes/edges/hashtable,
	// just reparse the input and build up the structures again.
	int iIterations = 0;
retry:
	iEdges = 0;

	// parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		char szNodeID[4];
		char szConnectedNodes[50];
		sscanf (pCur, "%3s : %[a-z ]", szNodeID, szConnectedNodes);

		NODE *head = ht_get(table, szNodeID);
		//If we haven't seen this node before, allocate and add it to the table
		if (NULL == head)
		{
			head = (NODE *)malloc(sizeof(NODE));
			ht_add(table, szNodeID, (void *)head);

			strcpy(head->name, szNodeID);
			head->connections = NULL;
			head->iNodesInNode = 1;
		}

		char *tok;
		tok = strtok(szConnectedNodes, " ");
		while (tok != NULL)
		{
			// For each connected node, add it to the table if necessary and 
			NODE *con = ht_get(table, tok);
			if (NULL == con)
			{
				con = (NODE *)malloc(sizeof(NODE));
				ht_add(table, tok, (void *)con);

				strcpy(con->name, tok);
				//con->connected = (NODE **)malloc(BASE_CAPACITY * sizeof(NODE *));
				con->connections = NULL;
				con->iNodesInNode = 1;
			}

			connectNodes(head, con, edges, &iEdges);

			tok = strtok(NULL, " ");
		}
	}

	// Start: Find a minimum cut via Karger's algorithm, and if it is not the known
	// minimum cut size of three, clean up the graph and start over
	long prod = findPossibleMinimumCut(table, edges, &iEdges);

	if (iEdges != 3)
	{
		printf("Iteration %d: %d edges(incorrect), retrying...\n", ++iIterations, iEdges);

		// Clean up all of the connections in the remaining two nodes
		CONNECTION *c = edges[0]->n1->connections;
		while (c != NULL)
		{
			CONNECTION *next = c->next;
			free(c);
			c = next;
		}
		c = edges[0]->n2->connections;
		while (c != NULL)
		{
			CONNECTION *next = c->next;
			free(c);
			c = next;
		}

		// Clean up the remaining edges
		for (int i = 0; i < iEdges; i++)
			free(edges[i]);

		// Clean the hashtable
		ht_clear(table);

		rewind(pFile);

		// Try again...
		goto retry;
	}

	printf("mincut is %d, prod of dims is %ld\n", iEdges, prod);
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

	day25(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
