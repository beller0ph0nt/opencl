#ifndef __KERNEL_MAX_H
#define __KERNEL_MAX_H

#include "Kernel.h"

struct KernelMaxCalcParam : public KernelCalcParam
{
};

class KernelMax : public Kernel
{
public:
    KernelMax(Program *p):Kernel(p) {}
    ~KernelMax();

    void Calc(KernelCalcParam p);
};

#endif // __KERNEL_MAX_H
