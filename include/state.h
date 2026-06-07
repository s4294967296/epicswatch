#ifndef EW_H_STATE
#define EW_H_STATE

typedef struct {
	char mode;
	int time;
	int rows;
	int cols;
	float refresh_period; // seconds
    unsigned int pv_count;
	char* pv;
    char** multi_pvs;
	int data_pos;
	int data_size;
} State;

typedef struct {
	int max_rows;
	int max_graph_rows;
	int buffsize;

	int win_top_edge_offset;
	int win_left_edge_offset;
	int win_bottom_edge_offset;
	int win_right_edge_offset;

	int tick_gap_horizontal;
	int tick_gap_vertical;
	int horizontal_axis_lbl_size;
	int vertical_axis_lbl_size;

} GraphState;


#endif
