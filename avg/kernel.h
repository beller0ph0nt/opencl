#ifndef __KERNEL_H
#define __KERNEL_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#include "context.h"
#include "program.h"

typedef struct
{
    cl_kernel** kernels = NULL;
} kernel_t;

kernel_t* kernel_create(const context_t* context, const program_t* prog);
void kernel_clear(context_t* context, kernel_t* kernel);

#endif // __KERNEL_H
