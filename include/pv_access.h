#ifndef EW_H_PV_ACCESS
#define EW_H_PV_ACCESS

#include <stdio.h>
#include <stdlib.h>
#include <cadef.h>
#include "state.h"

float get_pv(const char* pv);

void query_data(State* state, float data[], unsigned int pv_index);


#endif
