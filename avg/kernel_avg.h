#ifndef __KERNEL_AVG_H
#define __KERNEL_AVG_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#include "kernel.h"
#include "context.h"

#define MIN_BLOCK_LEN 2

struct avg_par_info_t
{
    double* start;
    unsigned long int len;
    cl_mem mem;
};

struct avg_work_size_t
{
    size_t global;
    size_t local;
};

struct avg_work_info_t
{
    struct avg_work_size_t size;
};

struct avg_time_info_t
{
    cl_ulong queued;
    cl_ulong submit;
    cl_ulong start;
    cl_ulong end;
};

struct avg_block_t
{
    struct avg_par_info_t orig;

    struct avg_par_info_t left;
    struct avg_par_info_t right;
    struct avg_par_info_t out;
    struct avg_work_info_t work;
    struct avg_time_info_t time;
    cl_event event;
    cl_context context;
    cl_kernel kernel;
    cl_command_queue cmd;
};

struct avg_params_t {
    double* in;
    unsigned long int in_len;
    double* out;
    unsigned long int out_len;
};

void kernel_avg_calc(const context_t* context,
                     const kernel_t* kernel,
                     struct avg_params_t* params);

void* avg_thread_func(void* arg);

void* cpu_avg_thread_func(void* arg);

#endif // __KERNEL_AVG_H
