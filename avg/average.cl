#include "params.h"

__kernel void avg(__global FLOAT(V_LEN) *left,
                  __global FLOAT(V_LEN) *right,
                  __global FLOAT(V_LEN) *out)
{
    size_t id = get_global_id(0);
    out[id] = (left[id] + right[id]) / 2;
}
