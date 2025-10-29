#ifndef EW_H_STATE
#define EW_H_STATE

typedef struct {
	char mode;
	int time;
	int rows;
	int cols;
	float refresh_period; // seconds
	const char* pv;
	int data_pos;
	int data_size;
} State;

#endif
