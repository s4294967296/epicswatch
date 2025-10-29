#ifndef EW_H_PV_ACCESS
#define EW_H_PV_ACCESS

#include <stdio.h>
#include <stdlib.h>
#include <cadef.h>
#include "state.h"

float get_pv(const char* pv);

int query_data(State* state, float data[]);


#endif
