#ifndef __MAIN_CONTEXT_H
#define __MAIN_CONTEXT_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

typedef cl_int cl_int_t;
typedef cl_uint cl_uint_t;
typedef cl_device_id cl_device_id_t;
typedef cl_device_type cl_device_type_t;
typedef cl_platform_id cl_platform_id_t;
typedef cl_context cl_context_t;
typedef cl_context_properties cl_context_properties_t;
typedef cl_command_queue cl_command_queue_t;

class MainContext
{
public:
    cl_uint_t total_dev_count = 0;
    cl_uint_t* dev_count = NULL;
    cl_device_id_t **dev = NULL;

    cl_uint_t plat_count = 0;
    cl_platform_id_t *plat = NULL;

    cl_context_t **context = NULL;
    cl_command_queue_t **cmd = NULL;
public:
    MainContext(const cl_device_type_t dev_type = CL_DEVICE_TYPE_GPU);
    ~MainContext();
};

#endif // __MAIN_CONTEXT_H
