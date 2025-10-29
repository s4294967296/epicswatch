#include <stdio.h>
#include <stdlib.h>
#include <cadef.h>
#include "pv_access.h"


float get_pv(const char* pv) {
    const char *pvName = pv;
    chid pvChid;
    double value;
    int status;

    status = ca_context_create(ca_disable_preemptive_callback);
    if (status != ECA_NORMAL) {
        fprintf(stderr, "ca_context_create failed: %s\n", ca_message(status));
        return 1;
    }

    status = ca_create_channel(pvName, NULL, NULL, 0, &pvChid);
    if (status != ECA_NORMAL) {
        fprintf(stderr, "ca_create_channel failed: %s\n", ca_message(status));
        ca_context_destroy();
        return 1;
    }

    status = ca_pend_io(5.0);
    if (status != ECA_NORMAL) {
        fprintf(stderr, "Connection timeout for %s\n", pvName);
        ca_context_destroy();
        return 1;
    }

    status = ca_get(DBR_DOUBLE, pvChid, &value);
    if (status != ECA_NORMAL) {
        fprintf(stderr, "ca_get failed: %s\n", ca_message(status));
        ca_context_destroy();
        return 1;
    }

    ca_pend_io(1.0);

    ca_clear_channel(pvChid);
    ca_context_destroy();
    return value;
}


int query_data(const char* pv, float data[], int size, int pos) {
	data[pos] = get_pv(pv);
	
	if (pos == (size - 1)) {
		return 0;
	} else {
		return pos + 1;
	}
}

