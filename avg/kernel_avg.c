#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "err.h"
#include "free.h"
#include "kernel_avg.h"
#include "kernel_avg_params.h"

void kernel_avg_calc(const context_t* context,
                     const kernel_t* kernel,
                     struct avg_params_t* params)
{
    if (params->in_len < 2)
        return;

    int non_opencl_calc = 0;
    if (params->in_len < context->total_comp_units * MIN_BLOCK_LEN)
        non_opencl_calc = 1;

    non_opencl_calc = 1;    // DEBUG

    int plat = 0, dev = 0;
    unsigned long int block_len;
    double* cur_in_pointer = (double*) params->in;
    double* cur_out_pointer = (double*) params->out;
    unsigned long int remain_len = params->in_len;

    pthread_t threads[context->total_dev_count];

    if (non_opencl_calc == 1)
    {
        /*
         * Почему-то при выставлении длинны массива в 10 элементов
         * ОС полностью зависает и не подает признаков жизни.
         */

        printf("run calc in nonasync mode\n");

        struct avg_block_t block;
        block.context = context->contexts[plat][dev];
        block.kernel = kernel->kernels[plat][dev];
        block.cmd = context->cmd[plat][dev];

        block_len = remain_len - 1;

        block.left.len = block_len;
        block.left.start = cur_in_pointer;

        block.right.len = block_len;
        block.right.start = cur_in_pointer + 1;

        block.out.len = block_len;
        block.out.start = cur_out_pointer;

        int i;
        for (i = 0; i < block.left.len; i++)
        {
            block.out.start[i] = (block.left.start[i] + block.right.start[i]) / 2.0;
        }

        return;
    }
    else if (non_opencl_calc == 2)
    {
        printf("run calc in nonasync mode in one thread\n");

        struct avg_block_t block;
        block.context = context->contexts[plat][dev];
        block.kernel = kernel->kernels[plat][dev];
        block.cmd = context->cmd[plat][dev];


        block.orig.len = remain_len;
        block.orig.start = cur_in_pointer;


        block_len = remain_len - 1;

        block.left.len = block_len;
        block.left.start = cur_in_pointer;

        block.right.len = block_len;
        block.right.start = cur_in_pointer + 1;

        block.out.len = block_len;
        block.out.start = cur_out_pointer;



        size_t work_items_count = block.out.len / AVG_V_LEN;
        if (block.out.len % AVG_V_LEN != 0)
            work_items_count++;

        size_t work_groups_count = work_items_count / kernel->prop[plat][dev].pref_work_group_size_mult;
        if (work_items_count % kernel->prop[plat][dev].pref_work_group_size_mult != 0)
            work_groups_count++;

        block.work.size.global = work_groups_count * kernel->prop[plat][dev].pref_work_group_size_mult;
        block.work.size.local = kernel->prop[plat][dev].pref_work_group_size_mult;


        avg_thread_func(&block);
        //pthread_create(&threads[0], NULL, avg_thread_func, &block);
        //pthread_join(threads[0], NULL);

        return;
    }
    else if (non_opencl_calc == 3)
    {
        printf("run calc in async mode on CPU\n");

        const int THREAD_COUNT = 4;

        pthread_t t[THREAD_COUNT];

        struct avg_block_t b[THREAD_COUNT];

        block_len = remain_len / THREAD_COUNT;

        int i;
        for (i = 0; i < THREAD_COUNT; i++)
        {
            if (i != THREAD_COUNT - 1)
            {
                printf("block len: %lu\n", block_len);

                b[i].left.len = block_len;
                b[i].left.start = cur_in_pointer;

                b[i].right.len = block_len;
                b[i].right.start = cur_in_pointer + 1;

                b[i].out.len = block_len;
                b[i].out.start = cur_out_pointer;

                remain_len -= block_len;
                cur_in_pointer += block_len;
                cur_out_pointer += block_len;
            }
            else
            {
                block_len = remain_len - 1;

                printf("block len: %lu\n", block_len);

                b[i].left.len = block_len;
                b[i].left.start = cur_in_pointer;

                b[i].right.len = block_len;
                b[i].right.start = cur_in_pointer + 1;

                b[i].out.len = block_len;
                b[i].out.start = cur_out_pointer;

                remain_len = 0;
                cur_in_pointer = NULL;
                cur_out_pointer = NULL;
            }


        }

        for (i = 0; i < THREAD_COUNT; i++)
        {
            if (pthread_create(&t[i],
                               NULL,
                               cpu_avg_thread_func,
                               &b[i]) == 0)
            {
                printf("pthread_create [ok]\n");
            }
            else
            {
                printf("pthread_create [error]\n");
            }
        }

        for (i = 0; i < THREAD_COUNT; i++)
        {
            if (pthread_join(t[i], NULL) == 0)
            {
                printf("pthread_join [ok]\n");
            }
            else
            {
                printf("pthread_join [error]\n");
            }
        }







//        size_t work_items_count = b.out.len / AVG_V_LEN;
//        if (b.out.len % AVG_V_LEN != 0)
//            work_items_count++;

//        size_t work_groups_count = work_items_count / kernel->prop[plat][dev].pref_work_group_size_mult;
//        if (work_items_count % kernel->prop[plat][dev].pref_work_group_size_mult != 0)
//            work_groups_count++;

//        b.work.size.global = work_groups_count * kernel->prop[plat][dev].pref_work_group_size_mult;
//        b.work.size.local = kernel->prop[plat][dev].pref_work_group_size_mult;


//        avg_thread_func(&b);
        //pthread_create(&threads[0], NULL, avg_thread_func, &block);
        //pthread_join(threads[0], NULL);

        return;
    }



    double multiplier = 0.0;
    struct avg_block_t** blocks = malloc(sizeof(*blocks) * context->plat_count);


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
                multiplier = (double) context->dev_prop[plat][dev].max_comp_units / context->total_comp_units;
                block_len = (unsigned long int) ceil(params->in_len * multiplier);

                printf("multiplier: %f\n", multiplier);
                printf("block len: %lu\n", block_len);



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
                block_len = remain_len - 1;

                printf("block len: %lu\n", block_len);

                blocks[plat][dev].left.len = block_len;
                blocks[plat][dev].left.start = cur_in_pointer;

                blocks[plat][dev].right.len = block_len;
                blocks[plat][dev].right.start = cur_in_pointer + 1;

                blocks[plat][dev].out.len = block_len;
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
    pthread_t thread_id = 0;
    //= pthread_self();

    struct avg_block_t* block = (struct avg_block_t*) arg;


    block->left.mem = clCreateBuffer(block->context,
                                     CL_MEM_READ_ONLY,
                                     sizeof(*block->left.start) * block->left.len,
                                     //sizeof(*block->orig.start) * block->orig.len,    //sizeof(*block->left.start) * block->left.len,
                                     NULL,
                                     &err);
    printf("[%lu] clCreateBuffer %lu\t [%s]\n", thread_id, sizeof(*block->left.start) * block->left.len, err_to_str(err));
    //printf("[%lu] clCreateBuffer %lu\t [%s]\n", thread_id, sizeof(*block->orig.start) * block->orig.len, err_to_str(err));

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
                               //sizeof(*block->orig.start) * block->orig.len,    //sizeof(*block->left.start) * block->left.len,
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



    printf("\nqueued time:\t%lld nsec\n", block->time.queued);
    printf("submit time:\t%lld nsec\n", block->time.submit);
    printf("start time:\t%lld nsec\n", block->time.start);
    printf("end time:\t%lld nsec\n\n", block->time.end);

    printf("kernel queued time:\t%lld nsec\t%.9f sec\n", block->time.submit - block->time.queued, (block->time.submit - block->time.queued) / 1000000000.0);
    printf("kernel submit time:\t%lld nsec\t%.9f sec\n", block->time.start - block->time.submit, (block->time.start - block->time.submit) / 1000000000.0);
    printf("kernel exec time:\t%lld nsec\t%.9f sec\n", block->time.end - block->time.start, (block->time.end - block->time.start) / 1000000000.0);
    printf("-------------------------------------------------\n");
    printf("kernel total time:\t%lld nsec\n", block->time.end - block->time.queued);
    printf("kernel total time:\t%.9f sec\n\n", (block->time.end - block->time.queued) / 1000000000.0);


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




void* cpu_avg_thread_func(void* arg)
{
    pthread_t thread_id = pthread_self();

    struct avg_block_t* block = (struct avg_block_t*) arg;


    struct timespec start, stop;
    long dsec, dnsec;

    clock_gettime(CLOCK_REALTIME, &start);

    int i;
    for (i = 0; i < block->left.len; i++)
    {
        block->out.start[i] = (block->left.start[i] + block->right.start[i]) / 2.0;
    }

    clock_gettime(CLOCK_REALTIME, &stop);
    dsec = stop.tv_sec - start.tv_sec;
    dnsec = stop.tv_nsec - start.tv_nsec;

    unsigned long diff = 1000000000 * dsec + dnsec;
    printf("\n\n[%lu] time: %.9f sec\t\n\n", thread_id, diff / 1000000000.0);

    return NULL;
}
