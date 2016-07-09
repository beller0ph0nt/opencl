#ifndef __CONTEXT_H
#define __CONTEXT_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

class Context
{
public:
    cl_uint total_dev_count = 0;
    cl_uint *dev_count = NULL;
    cl_device_id **dev = NULL;

    cl_uint plat_count = 0;
    cl_platform_id *plat = NULL;

    cl_context **context = NULL;
    cl_command_queue **cmd = NULL;
public:
    Context(const cl_device_type dev_type = CL_DEVICE_TYPE_GPU);
    ~Context();
};

#endif // __CONTEXT_H
