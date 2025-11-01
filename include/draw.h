#ifndef EW_DRAW
#define EW_DRAW

#include <errno.h>
#include <stdio.h>

#include "state.h"
#include "util.h"


//////////////////////
// static area
int max_rows = 0;
int graph_rows = 0;
int left_edge_offset = 5;
int top_edge_offset = 2;
int bottom_edge_offset = 5;

int tick_gap = 7;


//////////////////////
// high level function prototypes
void draw_help(State* state);
void draw_graph(State* state, float data[]);

void draw_header(char* buff, State* state);
void draw_bounds(char* buff, State* state);
void draw_data(char* buff, State* state, float data[]);
void draw_x_axis(char* buff, State* state, float min, float max);
void draw_y_axis(char* buff, State* state, float min, float max);


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

	graph_rows = max_rows - top_edge_offset - bottom_edge_offset;

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

	draw_y_axis(buff, state, min, max);

	
	for (int i = 0; i < len - 1; i++) {
		pos--;
		if (pos <= 0) {
			pos = len - 1;
		}

		float bin = (data[pos] - min) / ((max - min) / graph_rows);
		buff[state->cols * (top_edge_offset + graph_rows - (int)bin) + i + left_edge_offset + 1] = 'x';
	}


}


void draw_x_axis(char* buff, State* state, float min, float max) {

}


void draw_y_axis(char* buff, State* state, float min, float max) {
	const int lbl_width = left_edge_offset;

	int exponent_min = log10_int(min);
	int exponent_max = log10_int(max);
	int exponent = exponent_min > exponent_max ? exponent_min : exponent_max;

	int exponent_label_position = (
		state->rows - (top_edge_offset + bottom_edge_offset) + 2
		) * state->cols;

	char lbl[lbl_width + 1];
	snprintf(lbl, lbl_width + 1, "E%+d", exponent);
	strncpy(buff + exponent_label_position, lbl, lbl_width);
	
	float val = 0;
	int tick_row = top_edge_offset;
	int tick_label_position = 0;

	while (tick_row < state->rows - (top_edge_offset + bottom_edge_offset)) {
		memset(lbl, 0, lbl_width);
		val = max - (max - min) * (
			(float)tick_row - (float)top_edge_offset
			) / (float)graph_rows;

		tick_label_position = state->cols * tick_row;

		snprintf(lbl, lbl_width + 1, "%+1.2f", val / (float)exp10_int(exponent));
		strncpy(buff + tick_label_position, lbl, lbl_width);

		tick_row += (1 + tick_gap);
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
