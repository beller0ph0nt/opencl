#ifndef __KERNEL_AVG_H
#define __KERNEL_AVG_H

#include "kernel.h"
#include "context.h"

typedef struct {
    double* in;
    unsigned long int len;
} avg_params_t;

void kernel_avg_calc(const context_t* context,
                     const kernel_t* kernel,
                     avg_params_t* params);

#endif // __KERNEL_AVG_H
