#ifndef HW_UTIL
#define HW_UTIL

#include <stdlib.h>
#include <stdio.h>

#include <math.h>

int log10_int(float x);
int exp10_int(int x);

float fminarr(float x[], int n);
float fmaxarr(float x[], int n);

float favarr(float x[], int n);

float fvararr(float x[], int n);
float fskewarr(float x[], int n);
float fkurtarr(float x[], int n);

#endif
