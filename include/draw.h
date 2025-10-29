#ifndef EW_DRAW
#define EW_DRAW

#include <errno.h>
#include <stdio.h>

#include "state.h"
#include "pv_access.h"


//////////////////////
// high level function prototypes
void draw_help(State* state);
void draw_graph(State* state, float data[]);

void draw_header(char* buff, State* state);

/////////////////////
// lower level function prototypes
void clear_stdout(void);

void draw_hline(char* buff, int pos, int times, const char c);
void draw_vline(char* buff, int pos, int cols, int times, const char c);

/////////////////////
// high level function implementation
void draw_help(State* state) {
	printf("EPICSWATCH\tHelp\t2025\n");
}

void draw_graph(State* state, float data[]) {
	int max_rows = state->rows < state->cols ? state->rows : state->cols;
	max_rows -= 2; // account for header
	

	int buffsize = state->cols * max_rows + 1;
	char buff[buffsize];
	memset(buff, ' ', buffsize);
	buff[buffsize - 1] = '\0';
	
	draw_header(buff, state);

	draw_hline(buff, state->cols - 1, state->cols, '-');
	draw_vline(buff, state->cols + 3, state->cols, max_rows-2, '|');

	for (int i = 0; i < buffsize; i++) {
		printf("%c", buff[i]);
	}

	fflush(stdout);
}

void draw_header(char* buff, State* state) {
	char header_str[state->cols + 1];
	snprintf(header_str, state->cols, "EPICSWATCH WATCH %s", state->pv); 
	memcpy(buff, header_str, strlen(header_str));
}

/////////////////////
// lower level function implementation
void clear_stdout() {
	printf("\e[1;1H\e[2J");
}

void draw_hline(char* buff, int pos, int times, const char c) {
	if (times == 0) {
		return;
	}
	
	buff[pos + times] = c;

	draw_hline(buff, pos, times - 1, c);
}

void draw_vline(char* buff, int pos, int cols, int times, const char c) {
	if (times == 0) {
		return;
	}

	buff[pos + times*cols] = c;

	draw_vline(buff, pos, cols, times - 1, c);
}


#endif
