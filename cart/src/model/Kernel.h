#ifndef __KERNEL_H
#define __KERNEL_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#include "Program.h"

typedef cl_program cl_program_t;
typedef cl_kernel cl_kernel_t;

typedef struct {
    size_t  pref_work_group_size_mult;
} KernelProp;

class Kernel : public Program
{
private:
    cl_kernel_t **kernel = NULL;
    KernelProp **prop = NULL;
public:
    Kernel();
    ~Kernel();
};

#endif // __KERNEL_H
