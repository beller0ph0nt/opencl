#ifndef __MAIN_CONTEXT_H
#define __MAIN_CONTEXT_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

typedef int plat_index_t;
typedef int dev_index_t;

typedef cl_uint cl_uint_t;
typedef cl_device_id cl_device_id_t;
typedef cl_platform_id cl_platform_id_t;
typedef cl_context cl_context_t;
typedef cl_command_queue cl_command_queue_t;
typedef cl_context_properties cl_context_properties_t;
typedef cl_int cl_int_t;

struct MainContextData
{
    cl_uint_t total_dev_count = 0;
    cl_uint_t *dev_count;
    cl_device_id_t **dev;

    cl_uint_t plat_count = 0;
    cl_platform_id_t *plat;

    cl_context_t **context;
    cl_command_queue_t **cmd;
};

class MainContext
{
private:
    struct MainContextData *data;
public:
    MainContext(cl_device_type dev_type);
    ~MainContext();
};

#endif // __MAIN_CONTEXT_H
