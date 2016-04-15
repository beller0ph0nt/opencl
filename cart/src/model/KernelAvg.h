#ifndef __KERNEL_AVG_H
#define __KERNEL_AVG_H

#include "Kernel.h"

struct KernelAvgCalcParam : public KernelCalcParam
{
};

class KernelAvg : public Kernel
{
public:
    KernelAvg(Program *p):Kernel(p) {}
    ~KernelAvg();

    void Calc(KernelCalcParam p);
};

#endif // __KERNEL_AVG_H
