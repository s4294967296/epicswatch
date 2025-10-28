#ifndef EW_DRAW
#define EW_DRAW

#include <errno.h>
#include <stdio.h>

#include "state.h"
#include "pv_access.h"


//////////////////////
// high level function prototypes
void draw_help(State* state);
void draw_graph(State* state);


/////////////////////
// lower level function prototypes
void clear_stdout(void);
void draw_hline(char* buff, unsigned int pos, unsigned int times, const char c);

/////////////////////
// high level function implementation
void draw_help(State* state) {
	printf("EPICSWATCH\tHelp\t2025\n");
}

void draw_graph(State* state) {
	int max_rows = state->rows < state->cols ? state->rows : state->cols;
	char buff[state->cols * max_rows];

	printf("EPICSWATCH\tWATCH\t%s\n", state->pv);
	draw_hline(buff, 0, (unsigned int)state->cols, '-');
	printf("%s", buff);
	fflush(stdout);
}

/////////////////////
// lower level function implementation
void clear_stdout() {
	printf("\e[1;1H\e[2J");
}

void draw_hline(char* buff, unsigned int pos, unsigned int times, const char c) {
	if (times == 0) {
		return;
	}
	
	buff[pos + times] = c;

	times--;
	draw_hline(buff, pos, times, c);
}


#endif
