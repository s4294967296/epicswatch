#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h> 
#include <sys/ioctl.h>

#include "state.h"
#include "pv_access.h"
#include "draw.h"

enum MODES {
	WATCH,
	HELP
};


int get_window_state(State* s);

void parse_args(int argc, char** argv);
bool string_starts_with(char* str, const char* seq);
void get_key_val(char* str, char** key, char** val);
void set_state(char* key, char* val);

bool parse_short_long_form(char* str, const char* s, const char* l, bool relaxed);

State state = {
	0,			// mode
	60,			// time
	0,			// rows
	0,			// cols
	1,		// refresh_period
	"NULL",		// pv
	0,			// data_pos
	0			// data_size
};


int main(int argc, char **argv) {
	int win_succ = get_window_state(&state);
	
	if (win_succ != 0) {
		return -1;
	}

	parse_args(argc, argv);

	int data_size = state.cols - 10;
	float data[data_size] = {};
	int data_pos = 0;
	state.data_size = data_size;

	int sec = (int)state.refresh_period;
	struct timespec refresh_period = {sec, (int)((state.refresh_period - sec)*1e9)};
	
	if (state.mode == HELP) {
		draw_help(&state);
	}
	else {
		while(true) {
			clear_stdout();
			
			state.data_pos = query_data(&state, data);

			draw_graph(&state, data);
			
			nanosleep(&refresh_period, nullptr);
		}
	}

	return 0;
}


int get_window_state(State* s) {
	struct winsize w = {};
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != 0) {
		int errnoioctl = errno;
		printf("ERROR: ioctl failed upon retrieving win size!\n");
		printf("errno: %d\n", errnoioctl);
		
		return -1;
	}
	
	s->rows = w.ws_row;
	s->cols = w.ws_col;
	return 0;
}


void parse_args(int argc, char **argv) {
	char* arg;
	for (int i = 1; i < argc; i++) {
		// - filter out "-" and "--" modifiers
		// - other argv are interpreted as in-order 
		// - use '='

		arg = argv[i];

		int sp = 0;
		if (string_starts_with(arg, "--")) {
			sp = 2;
		} else if (string_starts_with(arg, "-")) {
			sp = 1;
		}

		if (sp == 0) {
			continue;
		}

		char* key = nullptr; 
		char* val = nullptr;
		
		get_key_val(&arg[sp], &key, &val);
		set_state(key, val);
	}
}

bool string_starts_with(char* str, const char* seq) {
	if (str == nullptr || seq == nullptr) {return false;}

	if (strlen(str) < strlen(seq)) {return false;}

	int res = strncmp((const char*)str, seq, strlen(seq));
	if (res == 0) {
		return true;
	} 

	return false;
}

void get_key_val(char* str, char** key, char** val) {
	*key = str;
	char* eq_pos = strstr(str, "=");
	if (eq_pos == nullptr) {return;}
	
	eq_pos[0] = '\0';
	*val = eq_pos + 1;

	printf("%s\n", *key);
	printf("%s\n", *val);
}

bool parse_short_long_form(char* str, const char* s, const char* l, bool relaxed) {
	if ((string_starts_with(str, s) && strlen(str) == strlen(s)) ||
		(string_starts_with(str, l) && (relaxed || (strlen(str) == strlen(l))))) {
		return true;
	}
	return false;
}

void set_state(char* key, char* val) {
	if (key == nullptr) {return;}
	
	if (val == nullptr) {
		if (parse_short_long_form(key, "h", "help", true)) {
			state.mode = HELP;
			goto set_state_ret;
		}
	} else {
		if (parse_short_long_form(key, "t", "time", false)) {
			state.time = atoi(val);
			goto set_state_ret;
		}
		if (parse_short_long_form(key, "pv", "process_variable", false)) {
			state.pv = val;
			goto set_state_ret;
		}
		if (parse_short_long_form(key, "p", "period", false)) {
			state.refresh_period = atof(val);
			goto set_state_ret;
		}
	}
	set_state_ret:
	// possible future callbacks etc.
	return;
}

