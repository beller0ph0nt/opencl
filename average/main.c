#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <stdio.h>
#include <CL/cl.h>
#include <stdlib.h>
#include <string.h>

#include "kernel.h"

#define KERNELS_COUNT   1
#define DATA_SIZE       10

int main()
{
    unsigned int in_count = 1;
    float in_data[DATA_SIZE] = { 0 };
    float out_data[DATA_SIZE - 1] = { 0 };

    int len;
    char fname[] = "average.cl";
    if (kernel_len(fname, &len) == 0)
    {
        printf("%s len: %d byte\n", fname, len);
    }
    else
    {
        return 1;
    }

    char ** kernel_src = NULL;
    kernel_src = malloc(sizeof(char *) * KERNELS_COUNT);
    kernel_src[0] = malloc(sizeof(char) * (len + 1));
    kernel_src[0][len] = 0;
    if (kernel_read(fname, len, kernel_src[0]) == 0)
    {
        printf("--BEGIN--\n");
        printf("%s", kernel_src[0]);
        printf("--END--\n");
    }
    else
    {
        return 1;
    }

    printf("sizeof(kernel_src): %d\n", strlen(kernel_src[0]));


    int i;

    for (i = 0; i < DATA_SIZE; i++)
    {
        in_data[i] = i;
    }

    cl_platform_id  platform_id;
    cl_uint         num_of_platforms = 0;
    if (clGetPlatformIDs(1, &platform_id, &num_of_platforms) != CL_SUCCESS)
    {
        printf("Unable to get platform_id\n");
        return 1;
    }

    cl_device_id    device_id;
    cl_uint         num_of_devices = 0;
    if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 1, &device_id, &num_of_devices) != CL_SUCCESS)
    {
        printf("Unable to get device_id\n");
        return 1;
    }

    cl_context_properties properties[3] = {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties) platform_id,
        0
    };

    cl_int      err;
    cl_context  context = clCreateContext(properties, 1, &device_id, NULL, NULL, &err);

    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, (cl_command_queue_properties) 0, &err);

    cl_program program = clCreateProgramWithSource(context, 1, (const char **) kernel_src, NULL, &err);

    free(kernel_src[0]);
    free(kernel_src);

    if (clBuildProgram(program, 0, NULL, NULL, NULL, NULL) != CL_SUCCESS)
    {
        printf("Error building program\n");

        char buffer[4096];
        size_t length;
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);

        printf("%s\n", buffer);

        return 1;
    }

    cl_kernel kernel = clCreateKernel(program, "num_threshold", &err);

    cl_mem input  = clCreateBuffer(context, CL_MEM_READ_ONLY,  sizeof(float) * DATA_SIZE,       NULL, NULL);
    cl_mem count  = clCreateBuffer(context, CL_MEM_READ_ONLY,  sizeof(unsigned int),            NULL, NULL);
    cl_mem output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * (DATA_SIZE - 1), NULL, NULL);

    clEnqueueWriteBuffer(command_queue, input, CL_TRUE, 0, sizeof(float) * DATA_SIZE, in_data,   0, NULL, NULL);
    clEnqueueWriteBuffer(command_queue, count, CL_TRUE, 0, sizeof(unsigned int),      &in_count, 0, NULL, NULL);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &count);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &output);

    size_t global = DATA_SIZE - 1;
    clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global, NULL, 0, NULL, NULL);
    clFinish(command_queue);

    clEnqueueReadBuffer(command_queue, output, CL_TRUE, 0, sizeof(float) * (DATA_SIZE - 1), out_data, 0, NULL, NULL);

    printf("output: ");
    for(i = 0; i < DATA_SIZE - 1; i++)
    {
        printf("%f ", out_data[i]);
    }
    printf("\n");

    clReleaseMemObject(output);
    clReleaseMemObject(count);
    clReleaseMemObject(input);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);

    return 0;
}
