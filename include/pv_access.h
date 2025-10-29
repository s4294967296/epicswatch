#ifndef PV_ACCESS
#define PV_ACCESS

#include <stdio.h>
#include <stdlib.h>
#include <cadef.h>

float get_pv(const char* pv);

int query_data(const char* pv, float data[], int size, int pos);


#endif
