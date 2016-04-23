#ifndef __PROGRAM_H
#define __PROGRAM_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#include "context.h"

typedef struct
{
    cl_program **avg_programs = NULL;
} program_t;

#ifdef __cplusplus
extern "C" {
#endif

program_t* program_create_src(const context_t* context, const char* src_path);
program_t* program_create_bin(const context_t* context, const char* bin_path);
void program_clear(context_t* context, program_t* prog);

#ifdef __cplusplus
}
#endif

#endif // __PROGRAM_H
