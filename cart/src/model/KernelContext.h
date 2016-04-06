#ifndef __KERNEL_CONTEXT_H
#define __KERNEL_CONTEXT_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

typedef cl_program cl_program_t;
typedef cl_kernel cl_kernel_t;

struct KernelContextData
{
    cl_program_t **avg_programs = NULL;
    cl_kernel_t **avg_kernels = NULL;
};

class KernelContext
{
private:
    struct KernelContextData *data;
public:
    KernelContext() { data = new KernelContextData; }
    ~KernelContext() { delete data; }
};

#endif // __KERNEL_CONTEXT_H
