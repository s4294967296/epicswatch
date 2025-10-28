#ifndef EW_STATE
#define EW_STATE

struct state {
	char mode;
	int time;
	int rows;
	int cols;
	float refresh_period; // seconds
	const char* pv;
} const state_default = {
	0,			// mode
	60,			// time
	0,			// rows
	0,			// cols
	1.0,		// refresh_period
	"NULL"		// pv
};

typedef struct state State;

#endif
