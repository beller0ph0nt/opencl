kernel void num_threshold(constant float * in,
                          constant uint * count,
                          global float * out)
{
    size_t id = get_global_id(0);
    uint cnt = *count;
    uint i = id / cnt;
    uint r = id % cnt;
    uint e = cnt * i + r;

    out[e] = (in[e] + in[e + 1]) / 2;
}
