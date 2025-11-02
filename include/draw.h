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
void draw_x_axis(char* buff, State* state);
void draw_y_axis(char* buff, State* state, float min, float max);

/////////////////////
// lower level function prototypes
void clear_stdout(void);

void draw_hline(char* buff, int pos, int times, const char c);
void draw_vline(char* buff, int pos, int cols, int times, const char c);

void draw_grid(char* buff, State* state, const char c);
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

	draw_grid(buff, state, '.');
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
	draw_x_axis(buff, state);

	
	for (int i = 0; i < len - 1; i++) {
		pos--;
		if (pos <= 0) {
			pos = len - 1;
		}

		float bin = (data[pos] - min) / ((max - min) / graph_rows);
		buff[state->cols * (top_edge_offset + graph_rows - (int)bin) + i + left_edge_offset + 1] = 'x';
	}


}


void draw_x_axis(char* buff, State* state) {
	const int lbl_width = 5;
	const int timebase_lbl_width = 5;

	// let the timebase_flag determine the label formatting. The label is
	//	displayed like this:
	//	xx:yy
	//	Where xx and yy can correspond to seconds, mins, hours or days:
	//	mm:ss --> timebase_flag = 0
	//	hh:mm --> timebase_flag = 1
	//  dd:hh --> timebase_flag = 2
	//
	int timebase_flag = 0;
	char timebase_lbl[timebase_lbl_width + 1];
	snprintf(timebase_lbl, timebase_lbl_width + 1, "mm:ss");
	
	const float max_duration = (state->cols - left_edge_offset) * state->refresh_period;
	if (max_duration > (3600 - 1)) {
		timebase_flag += 1;
		snprintf(timebase_lbl, timebase_lbl_width + 1, "hh:mm");
	}
	if (max_duration > (3600 * 24 - 1)) {
		snprintf(timebase_lbl, timebase_lbl_width + 1, "dd:hh");
		timebase_flag += 1;
	}
	

	unsigned int time_small = 0;
	unsigned int time_large = 0;
	

	char lbl[lbl_width + 1];

	int tick_row = max_rows - bottom_edge_offset + 2;
	int tick_col = left_edge_offset + 4;
	int tick_label_position = 0;

	// timebase lbl
	strncpy(buff + state->cols * tick_row + left_edge_offset + 2, timebase_lbl, timebase_lbl_width);
	
	while (tick_col < state->cols - (left_edge_offset + lbl_width)) {
		memset(lbl, 0, lbl_width);

		// lbl_width / 2 accounts for centering the time on ':' of label
		unsigned int time = (tick_col + lbl_width / 2) * state->refresh_period;
		
		switch (timebase_flag) {
			case 0: {
				time_large = time / 60;
				time_small = time % 60;
				break;
			}
			case 1: {
				time_large = time / 3600;
				time_small = (time % 3600) / 60;
				break;
			}
			case 2: {
				time_large = time / (3600 * 24);
				time_small = (time % (3600 * 24)) / 3600;
				break;
			}
			default: {
				// TODO: error 
			}
		}
		tick_label_position = state->cols * tick_row + (tick_col + left_edge_offset + 3);
		snprintf(lbl, lbl_width + 1, "%02u:%02u", time_large, time_small);

		strncpy(buff + tick_label_position, lbl, lbl_width);

		tick_col += (lbl_width + tick_gap);
	}
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


void draw_grid(char* buff, State* state, const char c) {
	// TODO: somehow encapsulate this into state.. this is due to lbl size
	int additional_gap_horizontal = 5;	
	for (int i = left_edge_offset;// + additional_gap_horizontal + tick_gap;
		 i < state->cols; 
		 i += tick_gap + additional_gap_horizontal) {
		for (int j = top_edge_offset; j < state->rows; j += tick_gap + 1) {
			buff[state->cols * j + i] = c;
		}
	}
}


#endif
