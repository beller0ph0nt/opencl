#ifndef __KERNEL_H
#define __KERNEL_H

#include "context.h"
#include "program.h"

#ifdef __cplusplus
extern "C" {
#endif

void kernel_create(const context_t* context, const program_t* prog);
void kernel_clear(const context_t* context, program_t* prog);

#ifdef __cplusplus
}
#endif

#endif // __KERNEL_H
