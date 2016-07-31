#ifndef __WRAP_OPENCL_H
#define __WRAP_OPENCL_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#define BUILD_LOG_LEN   4096

typedef
void (*pfn_create_context_notify_t)(const char*,
                                    const void*,
                                    size_t,
                                    void*);

typedef
void (*pfn_build_program_notify_t)(cl_program program,
                                   void*      user_data);

void
_w_clGetPlatformIDs(cl_uint         num_entries,
                    cl_platform_id* platforms,
                    cl_uint*        num_platforms);

void
_w_clGetDeviceIDs(cl_platform_id platform,
                  cl_device_type device_type,
                  cl_uint        num_entries,
                  cl_device_id*  devices,
                  cl_uint*       num_devices);

void
_w_clGetDeviceInfo(cl_device_id   device,
                   cl_device_info param_name,
                   size_t         param_value_size,
                   void*          param_value,
                   size_t*        param_value_size_ret);

cl_context
_w_clCreateContext(const cl_context_properties* properties,
                   cl_uint                      num_devices,
                   const cl_device_id*          devices,
                   pfn_create_context_notify_t  notify,
                   void*                        user_data);

cl_command_queue
_w_clCreateCommandQueue(cl_context                  context,
                        cl_device_id                device,
                        cl_command_queue_properties properties);

void
_w_clReleaseCommandQueue(cl_command_queue command_queue);

void
_w_clReleaseContext(cl_context context);

cl_program
_w_clCreateProgramWithSource(cl_context    context,
                             cl_uint       count,
                             const char**  strings,
                             const size_t* lengths);

void
_w_clBuildProgram(cl_program                 program,
                  cl_device_id               device,
                  cl_uint                    num_devices,
                  const cl_device_id*        device_list,
                  const char*                options,
                  pfn_build_program_notify_t notify,
                  void*                      user_data);

void
_w_clGetProgramBuildInfo(cl_program            program,
                         cl_device_id          device,
                         cl_program_build_info param_name,
                         size_t                param_value_size,
                         void*                 param_value,
                         size_t*               param_value_size_ret);

void
_w_clReleaseProgram(cl_program program);

cl_kernel
_w_clCreateKernel(cl_program  program,
                  const char* kernel_name);

void
_w_clGetKernelWorkGroupInfo(cl_kernel                 kernel,
                            cl_device_id              device,
                            cl_kernel_work_group_info param_name,
                            size_t                    param_value_size,
                            void*                     param_value,
                            size_t*                   param_value_size_ret);

void
_w_clReleaseKernel(cl_kernel kernel);

#endif // __WRAP_OPENCL_H
