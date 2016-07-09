#ifndef __WRAP_OPENCL_H
#define __WRAP_OPENCL_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#define BUILD_LOG_LEN   4096

void _wrp_clGetPlatformIDs(cl_uint num_entries, cl_platform_id *platforms, cl_uint *num_platforms);

void _wrp_clGetDeviceIDs(cl_platform_id platform, cl_device_type device_type, cl_uint num_entries, cl_device_id *devices, cl_uint *num_devices);

void _wrp_clGetDeviceInfo(cl_device_id device, cl_device_info param_name, size_t param_value_size, void *param_value, size_t *param_value_size_ret);

cl_context _wrp_clCreateContext(const cl_context_properties *properties, cl_uint num_devices, const cl_device_id *devices, void (*pfn_notify)(const char*, const void*, size_t, void*), void *user_data);

cl_command_queue _wrp_clCreateCommandQueue(cl_context context, cl_device_id device, cl_command_queue_properties properties);

void _wrp_clReleaseCommandQueue(cl_command_queue command_queue);

void _wrp_clReleaseContext(cl_context context);

cl_program _wrp_clCreateProgramWithSource(cl_context context, cl_uint count, const char **strings, const size_t *lengths);

void _wrp_clBuildProgram(cl_program program, cl_device_id device, cl_uint num_devices, const cl_device_id *device_list, const char *options, void (*pfn_notify)(cl_program program, void *user_data), void *user_data);

void _wrp_clGetProgramBuildInfo(cl_program program, cl_device_id device, cl_program_build_info param_name, size_t param_value_size, void *param_value, size_t *param_value_size_ret);

void _wrp_clReleaseProgram(cl_program program);

cl_kernel _wrp_clCreateKernel(cl_program program, const char *kernel_name);

void _wrp_clGetKernelWorkGroupInfo(cl_kernel kernel, cl_device_id device, cl_kernel_work_group_info param_name, size_t param_value_size, void *param_value, size_t *param_value_size_ret);

void _wrp_clReleaseKernel(cl_kernel kernel);

#endif // __WRAP_OPENCL_H
