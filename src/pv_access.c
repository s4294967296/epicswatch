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


void query_data(State* state, float data[], unsigned int pv_index) {
    const unsigned int comp_data_pos = state->data_pos + state->data_size * pv_index;
	data[comp_data_pos] = (float)get_pv(state->multi_pvs[pv_index]);
}

