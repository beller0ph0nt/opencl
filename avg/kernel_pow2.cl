#include "kernel_pow2_params.h"

__kernel void POW2_PROG_NAME(__global DOUBLE(AVG_V_LEN)* in,
                             __constant int pow,
                             __global DOUBLE(AVG_V_LEN)* out)
{
    size_t id = get_global_id(0);
    out[id] = pown(in[id], pow);
}
