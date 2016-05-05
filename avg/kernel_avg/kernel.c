#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../err.h"
#include "../free.h"

#include "params.h"
#include "kernel.h"

void kernel_avg_calc(const context_t* context,
                     const kernel_t* kernel,
                     struct avg_params_t* params)
{
    pthread_t threads[context->total_dev_count];

    double* cur_in_pointer = (double*) params->in;
    double* cur_out_pointer = (double*) params->out;
    unsigned long int remain_len = params->in_len;
    unsigned long int block_len = params->in_len / context->total_dev_count;

    struct avg_block_t** blocks = malloc(sizeof(*blocks) * context->plat_count);

    int plat, dev;
    cl_uint cur_device_index = 1;
    for (plat = 0; plat < context->plat_count; plat++)
    {
        blocks[plat] = malloc(sizeof(**blocks) * context->dev_on_plat[plat]);

        for (dev = 0; dev < context->dev_on_plat[plat]; dev++, cur_device_index++)
        {
            blocks[plat][dev].context = context->contexts[plat][dev];
            blocks[plat][dev].kernel = kernel->kernels[plat][dev];
            blocks[plat][dev].cmd = context->cmd[plat][dev];

            if (cur_device_index != context->total_dev_count)
            {
                blocks[plat][dev].left.len = block_len;
                blocks[plat][dev].left.start = cur_in_pointer;

                blocks[plat][dev].right.len = block_len;
                blocks[plat][dev].right.start = cur_in_pointer + 1;

                blocks[plat][dev].out.len = block_len;
                blocks[plat][dev].out.start = cur_out_pointer;

                remain_len -= block_len;
                cur_in_pointer += block_len;
                cur_out_pointer += block_len;
            }
            else
            {
                blocks[plat][dev].left.len = remain_len - 1;
                blocks[plat][dev].left.start = cur_in_pointer;

                blocks[plat][dev].right.len = remain_len - 1;
                blocks[plat][dev].right.start = cur_in_pointer + 1;

                blocks[plat][dev].out.len = remain_len - 1;
                blocks[plat][dev].out.start = cur_out_pointer;

                remain_len = 0;
                cur_in_pointer = NULL;
                cur_out_pointer = NULL;
            }

            size_t work_items_count = blocks[plat][dev].out.len / AVG_V_LEN;
            if (blocks[plat][dev].out.len % AVG_V_LEN != 0)
                work_items_count++;

            size_t work_groups_count = work_items_count / kernel->prop[plat][dev].pref_work_group_size_mult;
            if (work_items_count % kernel->prop[plat][dev].pref_work_group_size_mult != 0)
                work_groups_count++;

            blocks[plat][dev].work.size.global = work_groups_count * kernel->prop[plat][dev].pref_work_group_size_mult;
            blocks[plat][dev].work.size.local = kernel->prop[plat][dev].pref_work_group_size_mult;

            if (pthread_create(&threads[cur_device_index - 1],
                               NULL,
                               avg_thread_func,
                               &blocks[plat][dev]) == 0)
            {
                printf("pthread_create [ok]\n");
            }
            else
            {
                printf("pthread_create [error]\n");
            }
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

    free_ptr_2d((void**) blocks, context->plat_count);
}

void* avg_thread_func(void* arg)
{
    cl_int err;
    pthread_t thread_id = pthread_self();
    struct avg_block_t* block = (struct avg_block_t*) arg;

    block->left.mem = clCreateBuffer(block->context,
                                     CL_MEM_READ_ONLY,
                                     sizeof(*block->left.start) * block->left.len,
                                     NULL,
                                     &err);
    printf("[%lu] clCreateBuffer \t [%s]\n", thread_id, err_to_str(err));

    err = clSetKernelArg(block->kernel,
                         0,
                         sizeof(block->left.mem),
                         &block->left.mem);
    printf("[%lu] clSetKernelArg \t\t [%s]\n", thread_id, err_to_str(err));




    block->right.mem = clCreateBuffer(block->context,
                                      CL_MEM_READ_ONLY,
                                      sizeof(*block->right.start) * block->right.len,
                                      NULL,
                                      &err);
    printf("[%lu] clCreateBuffer [%s]\n", thread_id, err_to_str(err));

    err = clSetKernelArg(block->kernel,
                         1,
                         sizeof(block->right.mem),
                         &block->right.mem);
    printf("[%lu] clSetKernelArg [%s]\n", thread_id, err_to_str(err));




    block->out.mem = clCreateBuffer(block->context,
                                    CL_MEM_WRITE_ONLY,
                                    sizeof(*block->out.start) * block->out.len,
                                    NULL,
                                    &err);
    printf("[%lu] clCreateBuffer [%s]\n", thread_id, err_to_str(err));

    err = clSetKernelArg(block->kernel,
                         2,
                         sizeof(block->out.mem),
                         &block->out.mem);
    printf("[%lu] clSetKernelArg [%s]\n", thread_id, err_to_str(err));




    err = clEnqueueWriteBuffer(block->cmd,
                               block->left.mem,
                               CL_TRUE,
                               0,
                               sizeof(*block->left.start) * block->left.len,
                               block->left.start,
                               0,
                               NULL,
                               NULL);
    printf("[%lu] clEnqueueWriteBuffer [%s]\n", thread_id, err_to_str(err));

    err = clEnqueueWriteBuffer(block->cmd,
                               block->right.mem,
                               CL_TRUE,
                               0,
                               sizeof(*block->right.start) * block->right.len,
                               block->right.start,
                               0,
                               NULL,
                               NULL);
    printf("[%lu] clEnqueueWriteBuffer [%s]\n", thread_id, err_to_str(err));




    err = clEnqueueNDRangeKernel(block->cmd,
                                 block->kernel,
                                 1,
                                 NULL,
                                 &block->work.size.global,
                                 &block->work.size.local,
                                 0,
                                 NULL,
                                 &block->event);
    printf("[%lu] clEnqueueNDRangeKernel [%s]\n", thread_id, err_to_str(err));

    err = clFinish(block->cmd);
    printf("[%lu] clFinish [%s]\n", thread_id, err_to_str(err));




    err = clGetEventProfilingInfo(block->event,
                                  CL_PROFILING_COMMAND_QUEUED,
                                  sizeof(block->time.queued),
                                  &block->time.queued,
                                  NULL);
    printf("[%lu] clGetEventProfilingInfo CL_PROFILING_COMMAND_QUEUED [%s]\n", thread_id, err_to_str(err));

    err = clGetEventProfilingInfo(block->event,
                                  CL_PROFILING_COMMAND_SUBMIT,
                                  sizeof(block->time.submit),
                                  &block->time.submit,
                                  NULL);
    printf("[%lu] clGetEventProfilingInfo CL_PROFILING_COMMAND_SUBMIT [%s]\n", thread_id, err_to_str(err));

    err = clGetEventProfilingInfo(block->event,
                                  CL_PROFILING_COMMAND_START,
                                  sizeof(block->time.start),
                                  &block->time.start,
                                  NULL);
    printf("[%lu] clGetEventProfilingInfo CL_PROFILING_COMMAND_START [%s]\n", thread_id, err_to_str(err));

    err = clGetEventProfilingInfo(block->event,
                                  CL_PROFILING_COMMAND_END,
                                  sizeof(block->time.end),
                                  &block->time.end,
                                  NULL);
    printf("[%lu] clGetEventProfilingInfo CL_PROFILING_COMMAND_END [%s]\n", thread_id, err_to_str(err));




    err = clEnqueueReadBuffer(block->cmd,
                              block->out.mem,
                              CL_TRUE,
                              0,
                              sizeof(*block->out.start) * block->out.len,
                              block->out.start,
                              0,
                              NULL,
                              NULL);
    printf("[%lu] clEnqueueReadBuffer [%s]\n", thread_id, err_to_str(err));




    err = clReleaseMemObject(block->left.mem);
    printf("[%lu] clReleaseMemObject [%s]\n", thread_id, err_to_str(err));

    err = clReleaseMemObject(block->right.mem);
    printf("[%lu] clReleaseMemObject [%s]\n", thread_id, err_to_str(err));

    err = clReleaseMemObject(block->out.mem);
    printf("[%lu] clReleaseMemObject [%s]\n", thread_id, err_to_str(err));

    return NULL;
}
