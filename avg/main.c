#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <time.h>
#include <stdio.h>
#include <CL/cl.h>
#include <stdlib.h>
#include <string.h>

#include "kernel.h"
#include "params.h"

#define CLOCK_ID        CLOCK_PROCESS_CPUTIME_ID    // CLOCK_REALTIME
#define KERNELS_COUNT   1
#define DATA_SIZE       1000000
#define KERNEL_SRC      "average.cl"
#define BUILD_OPTIONS   "-I ./"

int init()
{
    int ret = 0x00;

    int len = 0;
    if (kernel_len(KERNEL_SRC, &len) == 0)
    {
        char ** kernel_src = NULL;
        kernel_src = malloc(sizeof(char *) * KERNELS_COUNT);
        kernel_src[0] = malloc(sizeof(char) * (len + 1));
        kernel_src[0][len] = 0;
        if (kernel_read(KERNEL_SRC, len, kernel_src[0]) == 0)
        {
        }
        else
        {
            ret |= 0x02;
        }
    }
    else
    {
        ret |= 0x01;
    }

    return ret;
}

void cpu_test()
{
    /*
    clock_gettime(CLOCK_ID, &start);

    for (i = 0; i < DATA_SIZE - 1; i++)
    {
        out_data[i] = (in_data[i] + in_data[i + 1]) / 2.0;
    }

    clock_gettime(CLOCK_ID, &stop);
    dsec = stop.tv_sec - start.tv_sec;
    dnsec = stop.tv_nsec - start.tv_nsec;
    printf("CPU: %f sec { sec: %ld, nsec: %ld }\n", dsec + (dnsec / 1000000000.0), dsec, dnsec);
    */
}

int main()
{
    struct timespec start, stop;
    long dsec, dnsec;

    float in_data[DATA_SIZE] = { 0 };
    float out_data[DATA_SIZE - 1] = { 0 };

    int len;
    if (kernel_len(KERNEL_SRC, &len) == 0)
    {
        /*
        printf("%s len: %d byte\n", KERNEL_SRC, len);
        */
    }
    else
    {
        return 1;
    }

    char ** kernel_src = NULL;
    kernel_src = malloc(sizeof(char *) * KERNELS_COUNT);
    kernel_src[0] = malloc(sizeof(char) * (len + 1));
    kernel_src[0][len] = 0;
    if (kernel_read(KERNEL_SRC, len, kernel_src[0]) == 0)
    {
        /*
        printf("--BEGIN--\n");
        printf("%s", kernel_src[0]);
        printf("--END--\n");
        */
    }
    else
    {
        return 1;
    }

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

//    printf("platform id: %f\n", platform_id);
//    printf("platforms available: %d\n", num_of_platforms);


    cl_device_id    device_id;
    cl_uint         num_of_devices = 0;
    if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_of_devices) != CL_SUCCESS)
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

    if (clBuildProgram(program, 0, NULL, BUILD_OPTIONS, NULL, NULL) != CL_SUCCESS)
    {
        printf("Error building program\n");

        char buffer[4096];
        size_t length;
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);

        printf("%s\n", buffer);

        return 1;
    }

    cl_kernel kernel = clCreateKernel(program, "avg", &err);

    cl_mem input_left  = clCreateBuffer(context, CL_MEM_READ_ONLY,  sizeof(float) * (DATA_SIZE - 1), NULL, NULL);
    cl_mem input_right = clCreateBuffer(context, CL_MEM_READ_ONLY,  sizeof(float) * (DATA_SIZE - 1), NULL, NULL);
    cl_mem output      = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * (DATA_SIZE - 1), NULL, NULL);

    clEnqueueWriteBuffer(command_queue, input_left,  CL_TRUE, 0, sizeof(float) * (DATA_SIZE - 1), in_data,     0, NULL, NULL);
    clEnqueueWriteBuffer(command_queue, input_right, CL_TRUE, 0, sizeof(float) * (DATA_SIZE - 1), in_data + 1, 0, NULL, NULL);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_left);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &input_right);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &output);

    size_t div = (DATA_SIZE - 1) / V_LEN;
    size_t global_work_size = ((DATA_SIZE - 1) % V_LEN == 0) ? div : div + 1;

    /*
     * Необходим алгоритм вычисления размера локальной группы.
     * Он должен быть кратен 64.
     */
    size_t local_work_size = 1;

    clock_gettime(CLOCK_ID, &start);

    clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);
    clFinish(command_queue);

    clock_gettime(CLOCK_ID, &stop);
    dsec = stop.tv_sec - start.tv_sec;
    dnsec = stop.tv_nsec - start.tv_nsec;
    printf("GPU SSE: %f sec { sec: %ld, nsec: %ld }\n", dsec + (dnsec / 1000000000.0), dsec, dnsec);

    clEnqueueReadBuffer(command_queue, output, CL_TRUE, 0, sizeof(float) * (DATA_SIZE - 1), out_data, 0, NULL, NULL);

    printf("global work size: %d\tlocal work size: %d\n", global_work_size, local_work_size);

    /*
    printf("output: ");
    for(i = 0; i < DATA_SIZE - 1; i++)
    {
        printf("%f ", out_data[i]);
    }
    printf("\n");
    */

    clReleaseMemObject(output);
    clReleaseMemObject(input_right);
    clReleaseMemObject(input_left);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);

    return 0;
}
