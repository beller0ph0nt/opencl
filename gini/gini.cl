typedef struct
{
    unsigned long len;
    double *array;
} array_t;

typedef struct
{
    array_t left;
    array_t right;
} block_t;

__kernel void gini_num(__global block_t *blocks,
                       __constant unsigned long blocks_len,
                       __global double *result)
{
    int i;
    int gid = get_global_id(0);
    block_t block = blocks[gid];

    double left_sum = 0;

    for (i = 0; i < block.left.len; i++)
    {
    left_sum += block.left.array[i] * block.left.array[i];
    }

    left_sum = left_sum / block.left.len;

    double right_sum = 0;

    for (i = 0; i < block.right.len; i++)
    {
    right_sum += block.right.array[i] * block.right.array[i];
    }

    right_sum = right_sum / block.right.len;

    result[gid] = left_sum + right_sum;
}
