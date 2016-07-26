#include <stdexcept>

#include "errors.h"
#include "wrap_opencl.h"

void _wrp_clGetPlatformIDs(cl_uint num_entries, cl_platform_id *platforms, cl_uint *num_platforms)
{
    cl_int err = clGetPlatformIDs(num_entries, platforms, num_platforms);

    if (err != CL_SUCCESS)
    {
        throw runtime_error("clGetPlatformIDs");
    }
}

void _wrp_clGetDeviceIDs(cl_platform_id platform, cl_device_type device_type, cl_uint num_entries, cl_device_id *devices, cl_uint *num_devices)
{
    cl_int err = clGetDeviceIDs(platform, device_type, num_entries, devices, num_devices);

    if (err != CL_SUCCESS)
    {
        throw runtime_error("clGetDeviceIDs");
    }
}

void _wrp_clGetDeviceInfo(cl_device_id device, cl_device_info param_name, size_t param_value_size, void *param_value, size_t *param_value_size_ret)
{
    cl_int err = clGetDeviceInfo(device, param_name, param_value_size, param_value, param_value_size_ret);

    if (err != CL_SUCCESS)
    {
        throw runtime_error("clGetDeviceInfo");
    }
}

cl_context _wrp_clCreateContext(const cl_context_properties *properties, cl_uint num_devices, const cl_device_id *devices, void (*pfn_notify)(const char*, const void*, size_t, void*), void *user_data)
{
    cl_int err = 0;
    cl_context context = clCreateContext(properties, num_devices, devices, pfn_notify, user_data, &err);

    if (err != CL_SUCCESS)
    {
        throw runtime_error("clCreateContext");
    }

    return context;
}

void _wrp_clReleaseContext(cl_context context)
{
    cl_int err = clReleaseContext(context);

    if (err != CL_SUCCESS)
    {
        throw runtime_error("clReleaseContext");
    }
}

cl_command_queue _wrp_clCreateCommandQueue(cl_context context, cl_device_id device, cl_command_queue_properties properties)
{
    cl_int err = 0;
    cl_command_queue queue = clCreateCommandQueue(context, device, properties, &err);

    if (err != CL_SUCCESS)
    {
        throw runtime_error("clCreateCommandQueue");
    }

    return queue;
}

void _wrp_clReleaseCommandQueue(cl_command_queue command_queue)
{
    cl_int err = clReleaseCommandQueue(command_queue);

    if (err != CL_SUCCESS)
    {
        throw runtime_error("clReleaseCommandQueue");
    }
}

cl_program _wrp_clCreateProgramWithSource(cl_context context, cl_uint count, const char **strings, const size_t *lengths)
{
    cl_int err = 0;
    cl_program program = clCreateProgramWithSource(context, count, strings, lengths, &err);

    if (err != CL_SUCCESS)
    {
        throw runtime_error("clCreateProgramWithSource");
    }

    return program;
}

void _wrp_clReleaseProgram(cl_program program)
{
    cl_int err = clReleaseProgram(program);

    if (err != CL_SUCCESS)
    {
        throw runtime_error("clReleaseProgram");

    }
}

void _wrp_clBuildProgram(cl_program program, cl_device_id device, cl_uint num_devices, const cl_device_id *device_list, const char *options, void (*pfn_notify)(cl_program program, void *user_data), void *user_data)
{
    cl_int err = clBuildProgram(program, num_devices, device_list, options, pfn_notify, user_data);

    if (err != CL_SUCCESS)
    {
        char log[BUILD_LOG_LEN];
        size_t len;
        _wrp_clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(log), log, &len);

        throw runtime_error("clBuildProgram");
    }
}

void _wrp_clGetProgramBuildInfo(cl_program program, cl_device_id device, cl_program_build_info param_name, size_t param_value_size, void *param_value, size_t *param_value_size_ret)
{
    cl_int err = clGetProgramBuildInfo(program, device, param_name, param_value_size, param_value, param_value_size_ret);

    if (err != CL_SUCCESS)
    {
        throw runtime_error("clGetProgramBuildInfo");
    }
}

cl_kernel _wrp_clCreateKernel(cl_program program, const char *kernel_name)
{
    cl_int err = 0;
    cl_kernel kernel = clCreateKernel(program, kernel_name, &err);

    if (err != CL_SUCCESS)
    {
        throw runtime_error("clCreateKernel");
    }

    return kernel;
}

void _wrp_clReleaseKernel(cl_kernel kernel)
{
    cl_int err = clReleaseKernel(kernel);

    if (err != CL_SUCCESS)
    {
        throw runtime_error("clReleaseKernel");
    }
}

void _wrp_clGetKernelWorkGroupInfo(cl_kernel kernel, cl_device_id device, cl_kernel_work_group_info param_name, size_t param_value_size, void *param_value, size_t *param_value_size_ret)
{
    cl_int err = clGetKernelWorkGroupInfo(kernel, device, param_name, param_value_size, param_value, param_value_size_ret);

    if (err != CL_SUCCESS)
    {
        throw runtime_error("clGetKernelWorkGroupInfo");
    }
}
