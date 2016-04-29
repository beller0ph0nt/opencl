#include <pthread.h>

#include "kernel_avg.h"

void kernel_avg_calc(const context_t* context,
                     const kernel_t* kernel,
                     struct avg_params_t *params)
{
    struct avg_block_t** blocks = NULL;

    double* cur_pointer = (double*) params->in;

    unsigned long int remain_len = params->len;
    unsigned long int block_len = params->len / context->total_dev_count;

    blocks = malloc(sizeof(*blocks) * context->plat_count);

    int plat, dev;
    cl_uint cur_device_index = 1;
    for (plat = 0; plat < platforms_count; plat++)
    {
        blocks[plat] = malloc(sizeof(**blocks) * context->dev_on_plat[plat]);

        for (dev = 0; dev < context->dev_on_plat[plat]; dev++, cur_device_index++)
        {
            if (cur_device_index != context->total_dev_count)
            {
                blocks[plat][dev].left.len = block_len;
                blocks[plat][dev].left.start = cur_pointer;

                blocks[plat][dev].right.len = block_len;
                blocks[plat][dev].right.start = cur_pointer + 1;

                blocks[plat][dev].out.len = block_len;
                blocks[plat][dev].out.start = malloc(sizeof(*blocks[plat][dev].out.start) * blocks[plat][dev].out.len);

                remain_len -= block_len;
                cur_pointer += block_len;
            }
            else
            {
                blocks[plat][dev].left.len = remain_len - 1;
                blocks[plat][dev].left.start = cur_pointer;

                blocks[plat][dev].right.len = remain_len - 1;
                blocks[plat][dev].right.start = cur_pointer + 1;

                blocks[plat][dev].out.len = remain_len - 1;
                blocks[plat][dev].out.start = malloc(sizeof(*blocks[plat][dev].out.start) * blocks[plat][dev].out.len);

                remain_len = 0;
                cur_pointer = NULL;
            }
        }

        // вызов нового потока
    }
}

void* thread_func(void* arg)
{
    struct avg_block_t* block = (struct avg_block_t*) arg;
}
