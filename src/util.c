#include "util.h"


int log10_int(float x) {
	int exponent = 0;
	
	if (x == 0) { return 0; }

	float abs = x >= 0 ? x : -1 * x;
	if (abs > 1) {
		while(abs > 10) {
			exponent += 1;
			abs /= 10;
		}
		return exponent;
	}

	while(abs < 1) {
		exponent -= 1;
		abs *= 10;
	}

	return exponent;
}


int exp10_int(int x) {
	if (x == 0) {
		return 1;
	}
	return exp10_int(x - 1) * 10;
}
