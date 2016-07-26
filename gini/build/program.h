#ifndef __PROGRAM_H
#define __PROGRAM_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#include "context.h"

typedef struct
{
    char *prog_name;
    cl_program **programs;
} program_t;

program_t* program_create_src(const context_t *context, const char *src_path, const char *prog_name, const char *build_options);

void program_clear(const context_t *context, program_t *prog);

#endif // __PROGRAM_H
