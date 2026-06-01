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


float fminarr(float x[], int n) {
	if (x == NULL) {
		printf("fminarr called with NULL ptr arr, exiting");
		exit(0);
	}

	float min = x[0];

	for (int i = 1; i < n; i++) {
		if (x[i] < min) {
			min = x[i];
		}
	}

	return min;
}


float fmaxarr(float x[], int n) {
	if (x == NULL) {
		printf("fmaxarr called with NULL ptr arr, exiting");
		exit(0);
	}

	float max = x[0];

	for (int i = 1; i < n; i++) {
		if (x[i] > max) {
			max = x[i];
		}
	}

	return max;
}


float favarr(float x[], int n) {
	if (x == NULL) {
		printf("favarr called with NULL ptr arr, exiting");
		exit(0);
	}
    
    float sum = x[0];
    
    for (int i = 1; i < n; i++) {
        sum += x[i];
    }

    return sum / n;
}


float fvararr(float x[], int n) {
    const float mean = favarr(x, n);
    
    float mean_of_square = 0;
    for (int i = 0; i < n; i++) {
        mean_of_square += x[i] * x[i];
    }
    
    mean_of_square = mean_of_square / n;
    
    return mean_of_square - mean * mean;
}


float fskewarr(float x[], int n) {
    const float mean = favarr(x, n);
    const float std = sqrtf(fvararr(x, n));

    float sum = x[0];

    for (int i = 1; i < n; i++) {
        float d = x[i] - mean;
        sum += d * d * d;
    }

    return (sum / n) / (std * std * std);
}


float fkurtarr(float x[], int n) {
    const float mean = favarr(x, n);
    const float var = fvararr(x, n);

    float sum = x[0];

    for (int i = 1; i < n; i++) {
        float d = x[i] - mean;
        sum += d * d * d * d;
    }

    return (sum / n) / (var * var);
}
