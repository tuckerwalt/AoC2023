#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define INPUTNAME "input.txt"
#define LINE_BUFFER_SIZE 50

typedef enum pulse_type
{
	LO,
	HI
} PULSE_TYPE;

typedef enum module_type
{
	BROADCAST,
	CONJUNCTION,
	FLIPFLOP,
	OUTPUT
} MODULE_TYPE;

typedef struct module
{
	char name[4];
	MODULE_TYPE type;

	//For flip-flop
	int bOn;

	//inputs only for conjunction modules
	struct module *inputs[10];
	//remembered pulses
	PULSE_TYPE inputtypes[10];
	int iInputs;
	int iHi;

	struct module *outputs[10];
	int iOutputs;

	char szOutputs[35];

	//For part two
	int bCriticalInput;
} MODULE;

typedef struct pulse
{
	PULSE_TYPE pulse;
	MODULE *target;
	MODULE *source;

	struct pulse *next;
} PULSE;

/// Linked list-based fifo queue 
typedef struct fifoqueue
{
	PULSE *first;
	PULSE *last;
} QUEUE;

void q_add(QUEUE *q, PULSE *p)
{
	p->next = NULL;
	if (q->first == NULL)
	{
		q->first = p;
		q->last = p;
	}
	else
	{
		q->last->next = p;
		q->last = p;
	}
}

PULSE *q_pop(QUEUE *q)
{
	PULSE *ret = NULL;
	if (q->first != NULL)
	{
		ret = q->first;
		if (q->first->next == NULL)
			q->last = NULL;

		q->first = q->first->next;
	}
	return ret;
}

//Find the index of this specific source module in the input list for m 
int findInput(MODULE *m, MODULE *source)
{
	for (int i = 0; i < m->iInputs; i++)
	{
		if (m->inputs[i] == source)
			return i;
	}
	printf("COULD NOT FIND CONJUNCTION INPUT FOR %s\n", source->name);
	return 0;
}

//Queue a pulse for each of the given module's outputs
void queuePulses(QUEUE *q, MODULE *mod, PULSE_TYPE pType)
{
	for (int i = 0; i < mod->iOutputs; i++)
	{
		PULSE *p = (PULSE *)malloc(sizeof(PULSE));
		p->target = mod->outputs[i];
		p->pulse = pType;
		p->source = mod;
		q_add(q, p);
	}
}

/**
 * Presses the button once, propagating pulses through all of the modules until
 * nothing happens
*/
void pressButton(MODULE *broadcaster, int *hi, int *lo, int iButtonPress)
{
	int iLo = 1;
	int iHi = 0;
	int iSourceIndex = 0;

	//FIFO queue 
	QUEUE *queue = (QUEUE *)calloc(1, sizeof(QUEUE));

	//Add the initial pulses from the broadcaster to the queue
	for (int i = 0; i < broadcaster->iOutputs; i++)
	{
		PULSE *p = (PULSE *)malloc(sizeof(PULSE));
		p->target = broadcaster->outputs[i];
		p->pulse = LO;
		p->source = broadcaster;
		q_add(queue, p);
	}

	PULSE *p = q_pop(queue);
	while (NULL != p)
	{
		MODULE *m = p->target;
		//printf("Pulsing %s to %s\n", p->pulse ? "HI" : "LO", m->name);

		if (p->pulse == LO)
		{
			iLo++;
		}
		else
		{
			iHi++;
		}

		switch(m->type)
		{
			case FLIPFLOP:
				if (p->pulse == LO)
				{
					if (m->bOn)
					{
						m->bOn = 0;
						queuePulses(queue, m, LO);
					}
					else
					{
						m->bOn = 1;
						queuePulses(queue, m, HI);
					}
				}
				break;
			case CONJUNCTION:
				
				iSourceIndex = findInput(m, p->source);
				//Keep a running count of how many remembered HI there are
				//Speeds up check later
				if (m->inputtypes[iSourceIndex] == LO && p->pulse == HI)
				{
					m->iHi++;
				}
				else if (m->inputtypes[iSourceIndex] == HI && p->pulse == LO)
				{
					m->iHi--;
				}

				//First, set the last-remembered pulse for this specific input
				m->inputtypes[iSourceIndex] = p->pulse;
				//Now check if all remembered inputs are HIGH
				if (m->iHi == m->iInputs)
				{
					queuePulses(queue, m, LO);
				}
				else
				{
					if (m->bCriticalInput)
					{
						//Print the number of presses that it took for this module
						printf("%s HIT on button %d\n", m->name, iButtonPress);
					}
					queuePulses(queue, m, HI);
				}
				break;
			case OUTPUT:
				break;
		}
		//Free this pulse request and get the next one from the queue
		free(p);
		p = q_pop(queue);
	}

	//printf("done! %d Hi %d Lo\n", iHi, iLo);
	*hi = iHi;
	*lo = iLo;
}

//Find a module by its two character name
MODULE *findModule(MODULE **knownMods, int iMods, char *name)
{
	for (int i = 0; i < iMods; i++)
	{
		if (!strcmp(knownMods[i]->name, name))
			return knownMods[i];
	}
	return NULL;
}

// Print all of the modules to console
void printModules(MODULE **knownMods, int iMods)
{
	for (int i = 0; i < iMods; i++)
	{
		if (knownMods[i]->type == FLIPFLOP)
		{
			printf("%%%s -> ", knownMods[i]->name);
			for (int j = 0; j < knownMods[i]->iOutputs; j++)
			{
				printf("%s, ", knownMods[i]->outputs[j]->name);
			}
			printf("\n");
		}
		else if (knownMods[i]->type == CONJUNCTION)
		{
			printf("&%s -> ", knownMods[i]->name);
			for (int j = 0; j < knownMods[i]->iOutputs; j++)
			{
				printf("%s, ", knownMods[i]->outputs[j]->name);
			}
			printf("\n");

			printf("inputs: ");
			for (int x = 0; x < knownMods[i]->iInputs; x++)
			{
				printf("%s, ", knownMods[i]->inputs[x]->name);
			}
			printf("\n");
		}
		else if (knownMods[i]->type == OUTPUT)
		{
			printf("OUT: %s\n", knownMods[i]->name);
		}
	}
}

/**
 * Given a description of modules:
 * 
 * broadcaster -> a, b, c
 * %a -> b
 * %b -> c
 * %c -> inv
 * &inv -> a
 * 
 * Pressing a "button" sends a LO pulse to the broadcaster module
 * The broadcaster sends the pulse that it receives to all output modules: a, b, c
 * '%'(flip-flop) modules hold a state: on or off
 *    When receiving a HIGH pulse, nothing happens
 *    When receiving a LO pulse, the module switches state and:
 *       If on, sends HIGH to each output
 *       If off, sends LO to each output
 * 
 * '&'(Conjuction) modules store the last received pulse from every INPUT 
 *    (defaults to LO). If ALL last-received pulses are HIGH, it sends LO,
 *     otherwise it sends HIGH
 * 
 * Multiple the number of LO and HIGH pulses sent from 1000 button presses together
 * 
 * For part two, determine the number of presses needed to make the 'rx' module 
 * receive a LO pulse.
 * Trick: looking at the input, there are four modules that feed into a conjunction 
 * for rx; after finding which # of presses each feeder requires to send a HI pulse,
 * I can LCM to find the total presses required.
 * 
 * in: pFile - An open file pointer, freed by the caller
*/
void day20(FILE *pFile)
{
	char szBuffer[LINE_BUFFER_SIZE] = { 0 };
	char *pCur = NULL;

	MODULE *knownModules[100];
	int iModules = 0;

	MODULE *broadcaster = NULL;

	//parse file lines
	while (NULL != (pCur = fgets(szBuffer, LINE_BUFFER_SIZE, pFile)))
	{
		//do something
		//Add each module parsed to an array of known modules,
		//Then iterate over each one and add the conjunction targets and targets for everything

		//char *tok;
		char outputs[50] = {0};
		if (pCur[0] == 'b')
		{
			//broadcaster 
			broadcaster = (MODULE *)calloc(1, sizeof(MODULE));

			sscanf(pCur, "broadcaster -> %[a-z, ]\n", broadcaster->szOutputs);
			//printf("parsing broadcaster outputs %s\n", broadcaster->szOutputs);
		}
		else
		{
			char descriptor[4] = {0};
			knownModules[iModules] = (MODULE *)calloc(1, sizeof(MODULE));

			sscanf(pCur, "%3s -> %[a-z, ]\n", descriptor, knownModules[iModules]->szOutputs);
			//printf("Parsing module %s : %s\n", descriptor, knownModules[iModules]->szOutputs);

			if (descriptor[0] == '%')
			{
				knownModules[iModules]->type = FLIPFLOP;
			}
			else if (descriptor[0] == '&')
			{
				knownModules[iModules]->type = CONJUNCTION;
				if (!strcmp(descriptor+1, "cd") ||
					!strcmp(descriptor+1, "rk") ||
					!strcmp(descriptor+1, "zf") ||
					!strcmp(descriptor+1, "qx"))
					knownModules[iModules]->bCriticalInput = 1;
			}

			strcpy(knownModules[iModules]->name, descriptor+1);
			iModules++;
		}
	}

	if (!feof(pFile))
	{
		printf("ERROR failed to read file! %s\n", strerror(errno));
		return;
	}

	//Iterate over the modules and add pointers
	for (int i = 0; i < iModules; i++)
	{
		MODULE *cur = knownModules[i];

		char *tok = strtok(cur->szOutputs, ", ");
		while (NULL != tok)
		{
			MODULE *out = findModule(knownModules, iModules, tok);
			if (NULL == out)
			{
				knownModules[iModules] = (MODULE *)calloc(1, sizeof(MODULE));
				strcpy(knownModules[iModules]->name, tok);
				knownModules[iModules]->type = OUTPUT;
				
				cur->outputs[cur->iOutputs++] = knownModules[iModules];
				iModules++;
				tok = strtok(NULL, ", ");
				continue;
			}

			//if this feeds into a Conjunction, add to its input list
			cur->outputs[cur->iOutputs++] = out;
			if (out->type == CONJUNCTION)
			{
				out->inputs[out->iInputs++] = cur;
			}

			tok = strtok(NULL, ", ");
		}
	}

	char *tok = strtok(broadcaster->szOutputs, ", ");
	while (NULL != tok)
	{
		MODULE *out = findModule(knownModules, iModules, tok);
		broadcaster->outputs[broadcaster->iOutputs++] = out;

		tok = strtok(NULL, ", ");
	}

	printModules(knownModules, iModules);

	int iLo = 0;
	int iHi = 0;
	long lTotLo = 0;
	long lTotHi = 0;
	//Now start pressing the button
	for (int i = 0; i < 5000; i++)
	{
		pressButton(broadcaster, &iHi, &iLo, i+1);
		lTotHi += (long)iHi;
		lTotLo += (long)iLo;
	}

	//printf("after 1000 buttons, %ld Hi %ld Lo\n", lTotHi, lTotLo);
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

	day20(pFile);

	if (pFile)
		fclose(pFile);
	free(szInputFile);

	return ret;
}
