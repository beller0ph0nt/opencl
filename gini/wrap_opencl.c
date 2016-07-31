#include "def.h"
#include "errors.h"
#include "wrap_opencl.h"

/*****************************************************************************/
/*****************************************************************************/
void
_w_clGetPlatformIDs(cl_uint         num_entries,
                    cl_platform_id* platforms,
                    cl_uint*        num_platforms)
{
    TRACE_DEBUG("begin");
    cl_int err = clGetPlatformIDs(num_entries, platforms, num_platforms);
    if (err != CL_SUCCESS)
    {
        TRACE_ERROR("clGetPlatformIDs %d", err);
        exit(OPENCL_ERROR);
    }
}

/*****************************************************************************/
/*****************************************************************************/
void
_w_clGetDeviceIDs(cl_platform_id platform,
                  cl_device_type device_type,
                  cl_uint        num_entries,
                  cl_device_id*  devices,
                  cl_uint*       num_devices)
{
    TRACE_DEBUG("begin");
    cl_int err = clGetDeviceIDs(platform,
                                device_type,
                                num_entries,
                                devices,
                                num_devices);
    if (err != CL_SUCCESS)
    {
        TRACE_ERROR("clGetDeviceIDs %d", err);
        exit(OPENCL_ERROR);
    }
}

/*****************************************************************************/
/*****************************************************************************/
void
_w_clGetDeviceInfo(cl_device_id   device,
                   cl_device_info param_name,
                   size_t         param_value_size,
                   void*          param_value,
                   size_t*        param_value_size_ret)
{
    TRACE_DEBUG("begin");
    cl_int err = clGetDeviceInfo(device,
                                 param_name,
                                 param_value_size,
                                 param_value,
                                 param_value_size_ret);
    if (err != CL_SUCCESS)
    {
        TRACE_ERROR("clGetDeviceInfo %d", err);
        exit(OPENCL_ERROR);
    }
}

/*****************************************************************************/
/*****************************************************************************/
cl_context
_w_clCreateContext(const cl_context_properties* properties,
                   cl_uint                      num_devices,
                   const cl_device_id*          devices,
                   pfn_create_context_notify_t  pfn_notify,
                   void*                        user_data)
{
    TRACE_DEBUG("begin");
    cl_int err = 0;
    cl_context context = clCreateContext(properties,
                                         num_devices,
                                         devices,
                                         pfn_notify,
                                         user_data,
                                         &err);
    if (err != CL_SUCCESS)
    {
        TRACE_ERROR("clCreateContext %d", err);
        exit(OPENCL_ERROR);
    }

    return context;
}

/*****************************************************************************/
/*****************************************************************************/
cl_command_queue
_w_clCreateCommandQueue(cl_context                  context,
                        cl_device_id                device,
                        cl_command_queue_properties properties)
{
    TRACE_DEBUG("begin");
    cl_int err = 0;
    cl_command_queue queue = clCreateCommandQueue(context,
                                                  device,
                                                  properties,
                                                  &err);
    if (err != CL_SUCCESS)
    {
        TRACE_ERROR("clCreateCommandQueue %d", err);
        exit(OPENCL_ERROR);
    }

    return queue;
}

/*****************************************************************************/
/*****************************************************************************/
void
_w_clReleaseCommandQueue(cl_command_queue command_queue)
{
    TRACE_DEBUG("begin");
    cl_int err = clReleaseCommandQueue(command_queue);
    if (err != CL_SUCCESS)
    {
        TRACE_ERROR("clReleaseCommandQueue %d", err);
        exit(OPENCL_ERROR);
    }
}

/*****************************************************************************/
/*****************************************************************************/
void
_w_clReleaseContext(cl_context context)
{
    TRACE_DEBUG("begin");
    cl_int err = clReleaseContext(context);
    if (err != CL_SUCCESS)
    {
        TRACE_ERROR("clReleaseContext %d", err);
        exit(OPENCL_ERROR);
    }
}

/*****************************************************************************/
/*****************************************************************************/
cl_program
_w_clCreateProgramWithSource(cl_context    context,
                             cl_uint       count,
                             const char**  strings,
                             const size_t* lengths)
{
    TRACE_DEBUG("begin");
    cl_int err = 0;
    cl_program program = clCreateProgramWithSource(context,
                                                   count,
                                                   strings,
                                                   lengths,
                                                   &err);
    if (err != CL_SUCCESS)
    {
        TRACE_ERROR("clCreateProgramWithSource %d", err);
        exit(OPENCL_ERROR);
    }

    return program;
}

/*****************************************************************************/
/*****************************************************************************/
void
_w_clBuildProgram(cl_program                 program,
                  cl_device_id               device,
                  cl_uint                    num_devices,
                  const cl_device_id*        device_list,
                  const char*                options,
                  pfn_build_program_notify_t pfn_notify,
                  void*                      user_data)
{
    TRACE_DEBUG("begin");
    cl_int err = clBuildProgram(program,
                                num_devices,
                                device_list,
                                options,
                                pfn_notify,
                                user_data);
    if (err != CL_SUCCESS)
    {
        char log[BUILD_LOG_LEN];
        size_t len;
        _w_clGetProgramBuildInfo(program,
                                   device,
                                   CL_PROGRAM_BUILD_LOG,
                                   sizeof(log),
                                   log,
                                   &len);
        TRACE_ERROR("%s", log);
        exit(OPENCL_ERROR);
    }
}

/*****************************************************************************/
/*****************************************************************************/
void
_w_clGetProgramBuildInfo(cl_program            program,
                         cl_device_id          device,
                         cl_program_build_info param_name,
                         size_t                param_value_size,
                         void*                 param_value,
                         size_t*               param_value_size_ret)
{
    TRACE_DEBUG("begin");
    cl_int err = clGetProgramBuildInfo(program,
                                       device,
                                       param_name,
                                       param_value_size,
                                       param_value,
                                       param_value_size_ret);
    if (err != CL_SUCCESS)
    {
        TRACE_ERROR("clGetProgramBuildInfo %d", err);
        exit(OPENCL_ERROR);
    }
}

/*****************************************************************************/
/*****************************************************************************/
void
_w_clReleaseProgram(cl_program program)
{
    TRACE_DEBUG("begin");
    cl_int err = clReleaseProgram(program);

    if (err != CL_SUCCESS)
    {
        TRACE_ERROR("clReleaseProgram %d", err);
        exit(OPENCL_ERROR);
    }
}

/*****************************************************************************/
/*****************************************************************************/
cl_kernel
_w_clCreateKernel(cl_program program, const char *kernel_name)
{
    TRACE_DEBUG("begin");
    cl_int err = 0;
    cl_kernel kernel = clCreateKernel(program, kernel_name, &err);
    if (err != CL_SUCCESS)
    {
        TRACE_ERROR("clCreateKernel %d", err);
        exit(OPENCL_ERROR);
    }

    return kernel;
}

/*****************************************************************************/
/*****************************************************************************/
void
_w_clGetKernelWorkGroupInfo(cl_kernel                 kernel,
                            cl_device_id              device,
                            cl_kernel_work_group_info param_name,
                            size_t                    param_value_size,
                            void *                    param_value,
                            size_t*                   param_value_size_ret)
{
    TRACE_DEBUG("begin");
    cl_int err = clGetKernelWorkGroupInfo(kernel,
                                          device,
                                          param_name,
                                          param_value_size,
                                          param_value,
                                          param_value_size_ret);
    if (err != CL_SUCCESS)
    {
        TRACE_ERROR("clGetKernelWorkGroupInfo %d", err);
        exit(OPENCL_ERROR);
    }
}

/*****************************************************************************/
/*****************************************************************************/
void
_w_clReleaseKernel(cl_kernel kernel)
{
    TRACE_DEBUG("begin");
    cl_int err = clReleaseKernel(kernel);
    if (err != CL_SUCCESS)
    {
        TRACE_ERROR("clReleaseKernel %d", err);
        exit(OPENCL_ERROR);
    }
}
