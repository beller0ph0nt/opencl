#ifndef __CONTEXT_H
#define __CONTEXT_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

//typedef struct
//{
//    size_t  max_param_size;
//    int ratio;
//} dev_prop_t;

typedef struct
{
    cl_uint total_dev_count = 0;
    cl_uint* dev_on_plat = NULL;
    cl_device_id** dev = NULL;
    //dev_prop_t** dev_prop = NULL;

    cl_uint plat_count = 0;
    cl_platform_id* plat = NULL;

    cl_context** contexts = NULL;
    cl_command_queue** cmd = NULL;

    cl_device_type dev_type = CL_DEVICE_TYPE_GPU;
} context_t;

#ifdef __cplusplus
extern "C" {
#endif

context_t* context_create(cl_device_type dev_type);
void context_clear(context_t* context);

#ifdef __cplusplus
}
#endif

#endif // __CONTEXT_H
