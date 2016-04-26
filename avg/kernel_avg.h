#ifndef __KERNEL_AVG_H
#define __KERNEL_AVG_H

#include "kernel.h"
#include "context.h"

typedef struct {
    double *start;
    unsigned long int len;
    cl_mem mem;
} par_info_t;

typedef struct {
    size_t global;
    size_t local;
} work_size_t;

typedef struct {
    work_size_t size;
} work_info_t;

typedef struct {
    cl_ulong queued;
    cl_ulong submit;
    cl_ulong start;
    cl_ulong end;
} time_info_t;

typedef struct {
    par_info_t left;
    par_info_t right;
    par_info_t out;
    work_info_t work;
    cl_event event;
    time_info_t time;
} block_info_t;

typedef struct {
    double* in;
    unsigned long int len;
} avg_param_t;

void kernel_avg_calc(const context_t* context, const kernel_t* kernel, avg_param_t* param);

#endif // __KERNEL_AVG_H
