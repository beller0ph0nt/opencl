#include "kernel_avg.h"

void kernel_avg_calc(const context_t* context,
                     const kernel_t* kernel,
                     avg_params_t *params)
{
    struct par_info_t
    {
        double* start;
        unsigned long int len;
        cl_mem mem;
    };

    struct work_size_t
    {
        size_t global;
        size_t local;
    };

    struct work_info_t
    {
        struct work_size_t size;
    };

    struct time_info_t
    {
        cl_ulong queued;
        cl_ulong submit;
        cl_ulong start;
        cl_ulong end;
    };

    struct block_t
    {
        struct par_info_t left;
        struct par_info_t right;
        struct par_info_t out;
        struct work_info_t work;
        cl_event event;
        struct time_info_t time;
    };

    struct block_t** blocks = NULL;

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
