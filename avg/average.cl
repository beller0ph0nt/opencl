__kernel void avg(__constant float16 * left,
                  __constant float16 * right,
                  __global float16 * out)
{
    size_t id = get_global_id(0);
    out[id] = (left[id] + right[id]) / 2;
}
