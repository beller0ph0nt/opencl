#ifndef __KERNEL_H
#define __KERNEL_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#include "Program.h"

struct KernelCalcParam
{
};

typedef struct
{
    size_t  pref_work_group_size_mult;
} KernelProp;

class Kernel
{
public:
    Kernel(Program *p):prog(p) {}
    ~Kernel();

    virtual void Calc(KernelCalcParam p) = 0;

private:
    cl_kernel **kernel = NULL;
    KernelProp **prop = NULL;
    Program *prog;
};

#endif // __KERNEL_H
