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

#define MAX_PLATFORMS   100
#define MAX_DEVICES     100

cl_uint *devices_on_platform = NULL;
cl_device_id **devices = NULL;

cl_uint platforms_count = 0;
cl_platform_id *platforms = NULL;

cl_context **contexts = NULL;
cl_command_queue **cmd_queues = NULL;

void free_ptr_2d(void **p, unsigned int len_2d)
{
    int i;

    if (p != NULL)
    {
        for (i = 0; i < len_2d; i++)
        {
            if (p[i] != NULL)
            {
                free(p[i]);
            }
        }

        free(p);
    }
}

void free_ptr_1d(void *p)
{
    if (p != NULL)
    {
        free(p);
    }
}

void clear()
{
//    free_ptr_2d((void **) cmd_queues, platforms_count);
//    free_ptr_2d((void **) contexts, platforms_count);
//    free_ptr_2d((void **) devices, platforms_count);
//    free_ptr_1d(devices_on_platform);
//    free_ptr_1d(platforms);

    int i;

    if (cmd_queues != NULL)
    {
        for (i = 0; i < platforms_count; i++)
        {
            if (cmd_queues[i] != NULL)
            {
                free(cmd_queues[i]);
            }
        }

        free(cmd_queues);
    }

    if (contexts != NULL)
    {
        for (i = 0; i < platforms_count; i++)
        {
            if (contexts[i] != NULL)
            {
                free(contexts[i]);
            }
        }

        free(contexts);
    }

    if (devices != NULL)
    {
        for (i = 0; i < platforms_count; i++)
        {
            if (devices[i] != NULL)
            {
                free(devices[i]);
            }
        }

        free(devices);
    }

    if (devices_on_platform != NULL)
    {
        free(devices_on_platform);
    }

    if (platforms != NULL)
    {
        free(platforms);
    }
}



int init()
{
    int i, j, ret = 0;

#ifdef DEBUG
    printf("init\n");
#endif
    if (clGetPlatformIDs(0, NULL, &platforms_count) == CL_SUCCESS)
    {
#ifdef DEBUG
        printf("platforms: %d\n", platforms_count);
#endif
        platforms = malloc(sizeof(*platforms) * platforms_count);
        if (clGetPlatformIDs(platforms_count, platforms, NULL) == CL_SUCCESS)
        {
            devices_on_platform = malloc(sizeof(*devices_on_platform) * platforms_count);

            devices = malloc(sizeof(*devices) * platforms_count);
            contexts = malloc(sizeof(*contexts) * platforms_count);
            cmd_queues = malloc(sizeof(*cmd_queues) * platforms_count);

            for (i = 0; i < platforms_count; i++)
            {
                if (clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &devices_on_platform[i]) == CL_SUCCESS)
                {
    #ifdef DEBUG
                    printf("platform: %d\t devices: %d\n", i, devices_on_platform[i]);
    #endif
                    devices[i] = malloc(sizeof(**devices) * devices_on_platform[i]);
                    contexts[i] = malloc(sizeof(**contexts) * devices_on_platform[i]);
                    cmd_queues[i] = malloc(sizeof(**cmd_queues) * devices_on_platform[i]);

                    if (clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, devices_on_platform[i], devices[i], NULL) == CL_SUCCESS)
                    {
                        cl_context_properties properties[3] = {
                            CL_CONTEXT_PLATFORM,
                            (cl_context_properties) platforms[i],
                            0
                        };

                        // Создаем очереди
                        for (j = 0; j < devices_on_platform[i]; j++)
                        {
    #ifdef DEBUG
                            printf("platform: %d\t cmd queue: %d\n", i, j);
    #endif
                            cl_int err;
                            contexts[i][j] = clCreateContext(properties, devices_on_platform[i], devices[i], NULL, NULL, &err);
                            cmd_queues[i][j] = clCreateCommandQueue(contexts[i][j], devices[i][j], (cl_command_queue_properties) 0, &err);
                        }
                    }
                    else
                    {
                        // !!! необходимо освободить выделенную память !!!
                        ret = 4;
                    }
                }
                else
                {
                    ret = 3;
                }
            }
        }
        else
        {
#ifdef DEBUG
            printf("clGetPlatformIDs [ ERROR ]\n");
#endif
            // !!! необходимо освободить выделенную память !!!
            ret = 2;
        }
    }
    else
    {
        ret = 1;
    }





//    cl_platform_id  platforms[MAX_PLATFORMS];

//    if (clGetPlatformIDs(MAX_PLATFORMS, platforms, &num_of_platforms) == CL_SUCCESS)
//    {

//        for (i = 0; i < num_of_platforms; i++)
//        {

//        }


//        cl_device_id    device_id[MAX_DEVICES];
//        cl_uint         num_of_devices = 0;
//        if (clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_GPU, MAX_DEVICES, device_id, &num_of_devices) == CL_SUCCESS)
//        {



//        }
//        else
//        {
//            printf("clGetDeviceIDs [ ERROR ]\n");
//            return 1;
//        }
//    }
//    else
//    {
//        printf("clGetPlatformIDs [ ERROR ]\n");
//        ret |= 0x04;
//    }



//    int len = 0;
//    if (kernel_len(KERNEL_SRC, &len) == 0)
//    {
//        char **kernel_src = NULL;
//        kernel_src = malloc(sizeof(char *) * KERNELS_COUNT);
//        kernel_src[0] = malloc(sizeof(char) * (len + 1));
//        kernel_src[0][len] = 0;
//        if (kernel_read(KERNEL_SRC, len, kernel_src[0]) == 0)
//        {
//        }
//        else
//        {
//            ret |= 0x02;
//        }
//    }
//    else
//    {
//        ret |= 0x01;
//    }

    return ret;
}

int avg(float *in, unsigned long int len)
{
    int ret = 0x00;
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
    init();
    clear();

    struct timespec start, stop;
    long dsec, dnsec;

    float in_data[DATA_SIZE] = { 0 };
    float out_data[DATA_SIZE - 1] = { 0 };

    int i;
    for (i = 0; i < DATA_SIZE; i++)
    {
        in_data[i] = i;
    }


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



    cl_platform_id  platform_id;
    cl_uint         num_of_platforms = 0;
    if (clGetPlatformIDs(1, &platform_id, &num_of_platforms) != CL_SUCCESS)
    {
        printf("Unable to get platform_id\n");
        return 1;
    }

    printf("platforms available: %d\n", num_of_platforms);


    cl_device_id    device_id[MAX_DEVICES];
    cl_uint         num_of_devices = 0;
    if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, MAX_DEVICES, device_id, &num_of_devices) != CL_SUCCESS)
    {
        printf("Unable to get device_id\n");
        return 1;
    }

    printf("devices available: %d\n", num_of_devices);

    cl_context_properties properties[3] = {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties) platform_id,
        0
    };

    cl_int      err;
    cl_context  context = clCreateContext(properties, 1, &device_id[0], NULL, NULL, &err);

    cl_command_queue command_queue = clCreateCommandQueue(context, device_id[0], /*(cl_command_queue_properties) 0*/ CL_QUEUE_PROFILING_ENABLE, &err);








    cl_program program = clCreateProgramWithSource(context, 1, (const char **) kernel_src, NULL, &err);

    free(kernel_src[0]);
    free(kernel_src);

    if (clBuildProgram(program, 0, NULL, BUILD_OPTIONS, NULL, NULL) != CL_SUCCESS)
    {
        printf("Error building program\n");

        char buffer[4096];
        size_t length;
        clGetProgramBuildInfo(program, device_id[0], CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);

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

    cl_event myEvent;


    clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, &myEvent);
    clFinish(command_queue);

    clock_gettime(CLOCK_ID, &stop);
    dsec = stop.tv_sec - start.tv_sec;
    dnsec = stop.tv_nsec - start.tv_nsec;
    printf("GPU SSE: %f sec { sec: %ld, nsec: %ld }\n", dsec + (dnsec / 1000000000.0), dsec, dnsec);


    cl_ulong start_time, end_time, queued_time, submit_time;
    clGetEventProfilingInfo(myEvent, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &queued_time, NULL);
    clGetEventProfilingInfo(myEvent, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &submit_time, NULL);
    clGetEventProfilingInfo(myEvent, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start_time, NULL);
    clGetEventProfilingInfo(myEvent, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end_time, NULL);
    //cl_ulong kernelExecTimeNs = end_time - start_time;
    printf("queued time: %lld nsec\n", queued_time);
    printf("submit time: %lld nsec\n", submit_time);
    printf("start time: %lld nsec\n", start_time);
    printf("end time: %lld nsec\n", end_time);

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
