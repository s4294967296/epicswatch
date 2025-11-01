#ifndef EW_DRAW
#define EW_DRAW

#include <errno.h>
#include <stdio.h>

#include "state.h"

//////////////////////
// static area
int max_rows = 0;
int left_edge_offset = 4;
int top_edge_offset = 2;
int bottom_edge_offset = 5;

//////////////////////
// high level function prototypes
void draw_help(State* state);
void draw_graph(State* state, float data[]);

void draw_header(char* buff, State* state);
void draw_bounds(char* buff, State* state);
void draw_data(char* buff, State* state, float data[]);


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
	max_rows = state->rows < state->cols ? state->rows : state->cols;
	max_rows -= 2; // account for header
	

	int buffsize = state->cols * max_rows + 1;
	char buff[buffsize];
	memset(buff, ' ', buffsize);
	buff[buffsize - 1] = '\0';
	
	draw_header(buff, state);
	draw_bounds(buff, state);

	draw_data(buff, state, data);

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

void draw_bounds(char* buff, State* state) {
	draw_hline(buff, state->cols - 1, state->cols, '-');
	draw_hline(buff, (max_rows - 4) * state->cols - 1, state->cols, '-');
	draw_vline(buff, state->cols + left_edge_offset, state->cols, max_rows - 6, '|');
}

void draw_data(char* buff, State* state, float data[]) {
	int pos = state->data_pos;
	int len = state->data_size;

	int graph_rows = max_rows - top_edge_offset - bottom_edge_offset;

	float min = data[0];
	float max = data[0];

	for (int i = 0; i < len; i++) {
		if (data[i] < min) {
			min = data[i];
		}
		if (data[i] > max) {
			max = data[i];
		}
	}

	if (min == max) {
		min -= 1;
		max += 1;
	}

	
	for (int i = 0; i < len; i++) {
		pos--;
		if (pos <= 0) {
			pos = len - 1;
		}

		float bin = (data[pos] - min) / ((max - min) / graph_rows);
		buff[state->cols * (top_edge_offset + (int)bin) + i + left_edge_offset + 1] = 'x';
	}

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
