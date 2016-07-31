typedef struct
{
    unsigned long len;
    double*       array;
} array_t;

typedef struct
{
    array_t left;
    array_t right;
} block_t;

/*****************************************************************************/
/*****************************************************************************/
double
gini_sum(array_t a)
{
    double sum = 0;
    unsigned long i;

    for (i = 0; i < a.len; i++)
        sum += a.array[i] * a.array[i];

    sum /= a.len;

    return sum;
}

/*****************************************************************************/
/*****************************************************************************/
__kernel
void gini_num(__global block_t*        blocks,
              __const unsigned long blocks_len,
              __global double*         result)
{
    int i;
    int gid = get_global_id(0);
    block_t block = blocks[gid];
    double left_sum = gini_sum(block.left);
    double right_sum = gini_sum(block.right);

    result[gid] = left_sum + right_sum;
}
