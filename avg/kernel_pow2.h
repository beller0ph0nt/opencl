#ifndef __KERNEL_POW2_H
#define __KERNEL_POW2_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#include "kernel.h"
#include "context.h"

struct pow2_par_info_t
{
    double* start;
    unsigned long int len;
    cl_mem mem;
};

struct pow2_work_size_t
{
    size_t global;
    size_t local;
};

struct pow2_work_info_t
{
    struct pow2_work_size_t size;
};

struct pow2_time_info_t
{
    cl_ulong queued;
    cl_ulong submit;
    cl_ulong start;
    cl_ulong end;
};

struct pow2_block_t
{
    struct pow2_par_info_t left;
    struct pow2_par_info_t right;
    struct pow2_par_info_t out;
    struct pow2_work_info_t work;
    struct pow2_time_info_t time;
    cl_event event;
    cl_context context;
    cl_kernel kernel;
    cl_command_queue cmd;
};

struct pow2_params_t {
    double* in;
    unsigned long int in_len;
    double* out;
    unsigned long int out_len;
};

void kernel_pow2_calc(const context_t* context,
                      const kernel_t* kernel,
                      struct pow2_params_t* params);

void* pow2_thread_func(void* arg);

#endif // __KERNEL_POW2_H
