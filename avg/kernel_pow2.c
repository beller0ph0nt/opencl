#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "err.h"
#include "free.h"
#include "kernel_pow2.h"
#include "kernel_pow2_params.h"

void kernel_pow2_calc(const context_t* context,
                      const kernel_t* kernel,
                      struct pow2_params_t* params)
{
    pthread_t threads[context->total_dev_count];

    double* cur_in_pointer = (double*) params->in;
    double* cur_out_pointer = (double*) params->out;
    unsigned long int remain_len = params->in_len;
    unsigned long int block_len = params->in_len / context->total_dev_count;

    struct pow2_block_t** blocks = malloc(sizeof(*blocks) * context->plat_count);

    // формирование блоков
    int plat, dev;
    cl_uint cur_device_index = 1;
    for (plat = 0; plat < context->plat_count; plat++)
    {
        blocks[plat] = malloc(sizeof(**blocks) * context->dev_on_plat[plat]);

        for (dev = 0; dev < context->dev_on_plat[plat]; dev++, cur_device_index++)
        {

        }
    }

    int i = 0;
    for (i = 0; i < context->total_dev_count; i++)
    {
        if (pthread_join(threads[i], NULL) == 0)
        {
            printf("pthread_join [ok]\n");
        }
        else
        {
            printf("pthread_join [error]\n");
        }
    }

}

void* pow2_thread_func(void* arg)
{
    return NULL;
}
