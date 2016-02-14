__kernel void num_threshold(__constant float * in,
                            __constant uint * count,
                            __global float * out)
{
    size_t id = get_global_id(0);
    uint cnt = *count;
    uint i = id / cnt;
    uint r = id % cnt;
    uint e = cnt * i + r;

    out[e] = (in[e] + in[e + 1]) / 2;
}
