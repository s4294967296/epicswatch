#ifndef EW_DRAW
#define EW_DRAW

#include <errno.h>
#include <stdio.h>

#include "state.h"

//////////////////////
// high level function prototypes
void draw_help(State* state);
void draw_graph(State* state);


/////////////////////
// lower level function prototypes
void clear_stdout(void);


/////////////////////
// high level function implementation
void draw_help(State* state) {
	printf("EPICSWATCH\tHelp\t2025\n");
}

void draw_graph(State* state) {
	printf("EPICSWATCH\tWATCH\t%s\n", state->pv);
}

/////////////////////
// lower level function implementation
void clear_stdout() {
	printf("\e[1;1H\e[2J");
}


#endif
