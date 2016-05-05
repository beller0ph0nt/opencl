#include "params.h"

__kernel void AVG_PROG_NAME(__global REAL(V_LEN) *left,
                            __global REAL(V_LEN) *right,
                            __global REAL(V_LEN) *out)
{
    size_t id = get_global_id(0);
    out[id] = (left[id] + right[id]) / 2;
}
