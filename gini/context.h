#ifndef __CONTEXT_H
#define __CONTEXT_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#define DEV_PER_CONTEXT 1

typedef struct
{
    cl_uint  max_comp_units;
    cl_ulong glob_mem_size;
    cl_ulong max_mem_alloc_size;
} dev_prop_t;

typedef struct
{
    cl_uint total_dev_count;
    cl_uint *dev_on_plat;
    cl_device_id **dev;
    dev_prop_t **dev_prop;

    cl_uint plat_count;
    cl_platform_id *plat;

    cl_context **contexts;
    cl_command_queue **cmd;

    cl_device_type dev_type;

    cl_uint total_comp_units;
} context_t;

context_t* context_create(cl_device_type dev_type);

void context_clear(context_t *context);

#endif // __CONTEXT_H
