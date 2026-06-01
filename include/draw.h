#ifndef EW_DRAW
#define EW_DRAW

#include <errno.h>
#include <stdio.h>

#include "state.h"
#include "util.h"


//////////////////////
// static area
GraphState graph_state = {
	0, // max_rows
	0, // max_graph_rows
	0, // buffsize
	
	2, // win_top_edge_offset
	5, // win_left_edge_offset
	5, // win_bottom_edge_offset
	0, // win_right_edge_offset
	
	7, // tick_gap_horizontal
	7, // tick_gap_vertical
	5, // horizontal_axis_lbl_size
	1  // vertical_axis_lbl_size
};

static const char *help_text = 
"EPICSWATCH Help 2025\n"
"\n"
"  Watch EPICS process variables and plot them in real time.\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n";

//////////////////////
// high level function prototypes
void draw_help(State* state);
void draw_graph(State* state, float data[]);

void draw_header(char* buff, State* state);
void draw_footer(char* buff, State* state, float data[]);
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
	printf(help_text);
}

void draw_graph(State* state, float data[]) {
	if (state->rows < state->cols) {
		graph_state.max_rows = state->rows - graph_state.win_top_edge_offset;
	} else {
		graph_state.max_rows = state->cols - graph_state.win_top_edge_offset;
	}
	
	if (graph_state.max_rows <= 0) {
		printf("graph_state.max_rows < 1. Returning.\n");
		return;
	}
	

	int buffsize = state->cols * graph_state.max_rows + 1;
	if (buffsize <= 0) {
		printf("graph_state.buffsize < 1. Returning.\n");
		return;
	}

	char buff[buffsize];
	memset(buff, ' ', buffsize);
	buff[buffsize ] = '\0';
	
	draw_header(buff, state);
	draw_bounds(buff, state);
    draw_footer(buff, state, data);

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

void draw_footer(char* buff, State* state, float data[]) {
    char footer_str[state->cols + 1];
    char av_str[state->cols + 1];
    char var_str[state->cols + 1];
    char skew_str[state->cols + 1];
    char kurt_str[state->cols + 1];
    
    const float av = favarr(data, state->data_size);
    const float var = fvararr(data, state->data_size);
    const float skew = fskewarr(data, state->data_size);
    const float kurt = fkurtarr(data, state->data_size);
	
    const float min = fminarr(data, state->data_size);
	const float max = fmaxarr(data, state->data_size);
	const int exponent_min = log10_int(min);
	const int exponent_max = log10_int(max);
	const int exponent = exponent_min > exponent_max ? exponent_min : exponent_max;
    
    snprintf(av_str, state->cols, "AVERAGE: %+2.2f ", av / (float)exp10_int(exponent));
    snprintf(var_str, state->cols, "VARIANCE: %+2.2f ", var / (float)exp10_int(exponent));
    snprintf(skew_str, state->cols, "SKEW: %+2.2f ", skew / (float)exp10_int(exponent));
    snprintf(kurt_str, state->cols, "KURTOSIS: %+2.2f ", kurt / (float)exp10_int(exponent));

    const int av_str_len = strlen(av_str);
    const int var_str_len = strlen(var_str);
    const int skew_str_len = strlen(skew_str);
    const int kurt_str_len = strlen(kurt_str);

    int aggregate_len = av_str_len;
    if (aggregate_len > state->cols) {
        return;
    }
    memcpy(buff + (state->cols) * (state->rows - 3) + 2 + aggregate_len - av_str_len, av_str, av_str_len);
    aggregate_len += var_str_len;

    if (aggregate_len > state->cols) {
        return;
    }
    memcpy(buff + (state->cols) * (state->rows - 3) + 2 + aggregate_len - var_str_len, var_str, var_str_len);
    aggregate_len += skew_str_len;
    
    if (aggregate_len > state->cols) {
        return;
    }
    memcpy(buff + (state->cols) * (state->rows - 3) + 2 + aggregate_len - skew_str_len, skew_str, skew_str_len);
    aggregate_len += kurt_str_len;
 
    if (aggregate_len > state->cols) {
        return;
    }
    memcpy(buff + (state->cols) * (state->rows - 3) + 2 + aggregate_len - kurt_str_len, kurt_str, kurt_str_len);
    aggregate_len += av_str_len;
}

void draw_bounds(char* buff, State* state) {
	draw_hline(buff, state->cols - 1, state->cols, '-');
	draw_hline(buff, (graph_state.max_rows - 4) * state->cols - 1, state->cols, '-');
	draw_vline(buff, state->cols + graph_state.win_left_edge_offset, state->cols, graph_state.max_rows - 6, '|');
}

void draw_data(char* buff, State* state, float data[]) {
	int pos = state->data_pos;
	int len = state->data_size;

	graph_state.max_graph_rows = graph_state.max_rows - graph_state.win_top_edge_offset - graph_state.win_bottom_edge_offset;

	float min = fminarr(data, len);
	float max = fmaxarr(data, len);

	if (min == max) {
		min -= 0.5;
		max += 0.5;
	}

	draw_y_axis(buff, state, min, max);
	draw_x_axis(buff, state);


	float previous_bin = 0.0;
	for (int i = 0; i < len - 1; i++) {
		pos--;
		if (pos <= 0) {
			pos = len - 1;
		}

		float bin = (data[pos] - min) / ((max - min) / graph_state.max_graph_rows);
		
		int marker_pos = state->cols * (graph_state.win_top_edge_offset + graph_state.max_graph_rows - (int)bin) + i + graph_state.win_left_edge_offset + 1;
		buff[marker_pos] = 'x';
		
		int bin_diff = (int)bin - (int)previous_bin;
		previous_bin = bin;

		if (!i) {
			continue;
		}
		
		if (bin_diff > 0) {
			draw_vline(buff, marker_pos, state->cols, bin_diff - 1, '|');
		} else {
			draw_vline(buff, marker_pos - state->cols * (-1 * bin_diff), state->cols, -1 * bin_diff - 1, '|');
		}

	}


}


void draw_x_axis(char* buff, State* state) {
	const int lbl_width = graph_state.horizontal_axis_lbl_size;
	const int timebase_lbl_width = 5;
	if (lbl_width < timebase_lbl_width) {
		printf("lbl_width < timebase_lbl_width. Returning.\n");
		return;
	}

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
	
	const float max_duration = (state->cols - graph_state.win_left_edge_offset) * state->refresh_period;
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

	// TODO: Magic numbers
	int tick_row = graph_state.max_rows - graph_state.win_bottom_edge_offset + 2;
	int tick_col = graph_state.win_left_edge_offset + 4;
	int tick_label_position = 0;

	// timebase lbl
	strncpy(buff + state->cols * tick_row + graph_state.win_left_edge_offset + 2, timebase_lbl, timebase_lbl_width);
	
	while (tick_col < state->cols - (graph_state.win_left_edge_offset + lbl_width)) {
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
		tick_label_position = state->cols * tick_row + (tick_col + graph_state.win_left_edge_offset + 3);
		snprintf(lbl, lbl_width + 1, "%02u:%02u", time_large, time_small);

		strncpy(buff + tick_label_position, lbl, lbl_width);

		tick_col += (lbl_width + graph_state.tick_gap_horizontal);
	}
}


void draw_y_axis(char* buff, State* state, float min, float max) {
	const int lbl_width = graph_state.win_left_edge_offset;

	int exponent_min = log10_int(min);
	int exponent_max = log10_int(max);
	int exponent = exponent_min > exponent_max ? exponent_min : exponent_max;

	int exponent_label_position = (
		state->rows - (graph_state.win_top_edge_offset + graph_state.win_bottom_edge_offset) + 2
		) * state->cols;

	char lbl[lbl_width + 1];
	snprintf(lbl, lbl_width + 1, "E%+d", exponent);
	strncpy(buff + exponent_label_position, lbl, lbl_width);
	
	float val = 0;
	int tick_row = graph_state.win_top_edge_offset;
	int tick_label_position = 0;

	while (tick_row < state->rows - (graph_state.win_top_edge_offset + graph_state.win_bottom_edge_offset)) {
		memset(lbl, 0, lbl_width);
		val = max - (max - min) * (
			(float)tick_row - (float)graph_state.win_top_edge_offset
			) / (float)graph_state.max_rows;

		tick_label_position = state->cols * tick_row;

		snprintf(lbl, lbl_width + 1, "%+1.2f", val / (float)exp10_int(exponent));
		strncpy(buff + tick_label_position, lbl, lbl_width);

		tick_row += (1 + graph_state.tick_gap_vertical);
	}

}


/////////////////////
// lower level function implementation
void clear_stdout() {
	printf("\e[1;1H\e[2J");
}

void draw_hline(char* buff, int pos, int times, const char c) {
	if (times <= 0) {
		return;
	}
	
	buff[pos + times] = c;

	draw_hline(buff, pos, times - 1, c);
}

void draw_vline(char* buff, int pos, int cols, int times, const char c) {
	if (times <= 0) {
		return;
	}

	buff[pos + times*cols] = c;

	draw_vline(buff, pos, cols, times - 1, c);
}


void draw_grid(char* buff, State* state, const char c) {
	for (int i = graph_state.win_left_edge_offset;// + additional_gap_horizontal + tick_gap;
		 i < state->cols; 
		 i += graph_state.tick_gap_horizontal + graph_state.horizontal_axis_lbl_size) {
		for (int j = graph_state.win_top_edge_offset; 
			 j < graph_state.max_graph_rows + graph_state.win_top_edge_offset + 1;
			 j += graph_state.tick_gap_vertical + graph_state.vertical_axis_lbl_size) {
			buff[state->cols * j + i] = c;
		}
	}
}


#endif
