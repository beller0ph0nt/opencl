#include "kernel_avg_params.h"

__kernel void AVG_PROG_NAME(__global DOUBLE(AVG_V_LEN) *left,
                            __global DOUBLE(AVG_V_LEN) *right,
                            __global DOUBLE(AVG_V_LEN) *out)
{
    size_t id = get_global_id(0);
    out[id] = (left[id] + right[id]) / 2;

/*
    size_t id_1 = get_global_id(0);
    size_t id_2 = id_1 + 1;
    double16 l = left[id_1];
    double16 r = (double16) (l.s1, l.s2, l.s3, l.s4, l.s5, l.s6, l.s7, l.s8, l.s9, l.sa, l.sb, l.sc, l.sd, l.se, l.sf, r.s0);
    out[id_1] = (l + r) / 2;
*/
}
