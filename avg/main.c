#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <time.h>
#include <stdio.h>
#include <CL/cl.h>
#include <stdlib.h>

#include "err.h"
#include "free.h"
#include "kernel.h"
#include "params.h"

#define CLOCK_ID        CLOCK_REALTIME  // CLOCK_PROCESS_CPUTIME_ID    //
#define KERNELS_COUNT   1

//#define DATA_SIZE       16384     // V_LEN * max_param_size
#define DATA_SIZE       4000000

#define KERNEL_SRC      "average.cl"
#define BUILD_OPTIONS   "-I ./"

#define MAX_PLATFORMS   100
#define MAX_DEVICES     100

struct dev_prop
{
    size_t  max_param_size;
};

cl_uint *devices_on_platform = NULL;
cl_device_id **devices = NULL;
struct dev_prop **devices_prop;

cl_uint platforms_count = 0;
cl_platform_id *platforms = NULL;

cl_context **contexts = NULL;
cl_command_queue **cmd_queues = NULL;



cl_kernel **avg_kernels = NULL;

void clear()
{
    int i, j;
    for (i = 0; i < platforms_count; i++)
    {
        for (j = 0; j < devices_on_platform[i]; j++)
        {
            clReleaseCommandQueue(cmd_queues[i][j]);
            clReleaseContext(contexts[i][j]);
        }
    }

    free_ptr_2d((void **) cmd_queues, platforms_count);
    free_ptr_2d((void **) contexts, platforms_count);
    free_ptr_2d((void **) devices_prop, platforms_count);
    free_ptr_2d((void **) devices, platforms_count);
    free_ptr_1d(devices_on_platform);
    free_ptr_1d(platforms);
}

int init(cl_device_type dev_type)
{
    int ret = 0;

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
            devices_prop = malloc(sizeof(*devices_prop) * platforms_count);
            contexts = malloc(sizeof(*contexts) * platforms_count);
            cmd_queues = malloc(sizeof(*cmd_queues) * platforms_count);


            int i;
            for (i = 0; i < platforms_count; i++)
            {
                if (clGetDeviceIDs(platforms[i], dev_type, 0, NULL, &devices_on_platform[i]) == CL_SUCCESS)
                {
#ifdef DEBUG
                    printf("platform: %d\t devices: %d\n", i, devices_on_platform[i]);
#endif
                    devices[i] = malloc(sizeof(**devices) * devices_on_platform[i]);
                    devices_prop[i] = malloc(sizeof(**devices_prop) * devices_on_platform[i]);
                    contexts[i] = malloc(sizeof(**contexts) * devices_on_platform[i]);
                    cmd_queues[i] = malloc(sizeof(**cmd_queues) * devices_on_platform[i]);

                    cl_int *err = NULL;

                    if (clGetDeviceIDs(platforms[i], dev_type, devices_on_platform[i], devices[i], NULL) == CL_SUCCESS)
                    {
                        cl_context_properties properties[3] = {
                            CL_CONTEXT_PLATFORM,
                            (cl_context_properties) platforms[i],
                            0
                        };

                        int j;
                        for (j = 0; j < devices_on_platform[i]; j++)
                        {
                            // !!! добавить обработку ошибок !!!
                            clGetDeviceInfo(devices[i][j],
                                           CL_DEVICE_MAX_PARAMETER_SIZE,
                                           sizeof(devices_prop[i][j].max_param_size),
                                           &devices_prop[i][j].max_param_size,
                                           NULL);
#ifdef DEBUG
                            printf("platform: %d\t cmd queue: %d\n", i, j);
#endif
                            contexts[i][j] = clCreateContext(properties, devices_on_platform[i], &devices[i][j], NULL, NULL, err);
                            if (err != NULL)
                            {
                                cmd_queues[i][j] = clCreateCommandQueue(contexts[i][j], devices[i][j], (cl_command_queue_properties) 0, err);
                                if (err == NULL)
                                {
                                    ret = 6;
                                    break;
                                }
                            }
                            else
                            {
                                ret = 5;
                                break;
                            }
                        }
                    }
                    else
                    {
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
            ret = 2;
        }
    }
    else
    {
        ret = 1;
    }

    // этот блок необходимо вынести во внешнюю функцию
    // после неудачной инициализации она должна очищать память
//    if (ret != 0)
//    {
//        clear();
//    }

    return ret;
}



int avg_init()
{
    int ret = 0;

    int len;
    if (kernel_len(KERNEL_SRC, &len) == 0)
    {
        char **kernel_src = NULL;
        kernel_src = malloc(sizeof(char *) * KERNELS_COUNT);
        kernel_src[0] = malloc(sizeof(char) * (len + 1));
        kernel_src[0][len] = 0;
        if (kernel_read(KERNEL_SRC, len, kernel_src[0]) == 0)
        {
#ifdef DEBUG
            char err_str[MAX_STR_ERR_LEN];
#endif
            int i;
            cl_int err;

            avg_kernels = malloc(sizeof(*avg_kernels) * platforms_count);
            for (i = 0; i < platforms_count; i++)
            {
                avg_kernels[i] = malloc(sizeof(**avg_kernels) * devices_on_platform[i]);

                int j;
                for (j = 0; j < devices_on_platform[i]; j++)
                {
                    cl_program program = clCreateProgramWithSource(contexts[i][j], 1, (const char **) kernel_src, NULL, &err);
                    if (err == CL_SUCCESS)
                    {
                        err = clBuildProgram(program, 0, NULL, BUILD_OPTIONS, NULL, NULL);
                        if (err == CL_SUCCESS)
                        {
                            avg_kernels[i][j] = clCreateKernel(program, "avg", &err);
                            if (err != CL_SUCCESS)
                            {
                                ret = 1;
                            }
                        }
                        else
                        {
                            ret = 1;
                            printf("Error building program\n");

                            char buffer[4096];
                            size_t length;
                            clGetProgramBuildInfo(program, devices[i][j], CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);

                            printf("%s\n", buffer);
                        }

                        clReleaseProgram(program);
                    }
                    else
                    {
                        ret = 1;
    #ifdef DEBUG
                        err_to_str(err, err_str);
                        printf("clCreateProgramWithSource [ %s ]\n", err_str);
    #endif
                        break;
                    }
                }
            }
        }
        else
        {
            ret = 1;
        }

        free_ptr_2d((void **) kernel_src, KERNELS_COUNT);
    }
    else
    {
        ret = 1;
    }

    return ret;
}

void avg_clear()
{
    int i, j;
    for (i = 0; i < platforms_count; i++)
    {
        for (j = 0; j < devices_on_platform[i]; j++)
        {
            clReleaseKernel(avg_kernels[i][j]);
        }
    }

    free_ptr_2d((void **) avg_kernels, platforms_count);
}

int avg_calc(float *in, unsigned long int len)
{
    int ret = 0;
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
#ifdef DEBUG
    char err_str[MAX_STR_ERR_LEN];
#endif
    struct timespec start, stop;
    long dsec, dnsec;

    //float in_data[DATA_SIZE] = { 0 };
    float *in_data = malloc(sizeof(*in_data) * DATA_SIZE);


    //float out_data[DATA_SIZE - 1] = { 0 };
    float *out_data = malloc(sizeof(*out_data) * (DATA_SIZE - 1));

    long int i;
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

    cl_command_queue command_queue = clCreateCommandQueue(context, device_id[0], CL_QUEUE_PROFILING_ENABLE, &err);




    /*
     * процесс инициализации ядра
     */

    int len;
    if (kernel_len(KERNEL_SRC, &len) == 0)
    {
//        printf("%s len: %d byte\n", KERNEL_SRC, len);
    }
    else
    {
        return 1;
    }

    char **kernel_src = NULL;
    kernel_src = malloc(sizeof(char *) * KERNELS_COUNT);
    kernel_src[0] = malloc(sizeof(char) * (len + 1));
    kernel_src[0][len] = 0;
    if (kernel_read(KERNEL_SRC, len, kernel_src[0]) == 0)
    {
//        printf("--BEGIN--\n");
//        printf("%s", kernel_src[0]);
//        printf("--END--\n");
    }
    else
    {
        return 1;
    }

    cl_program program = clCreateProgramWithSource(context, 1, (const char **) kernel_src, NULL, &err);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clCreateProgramWithSource [ %s ]\n", err_str);
#endif

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
    clReleaseProgram(program);





    /*
     * настройка переменных
     */

    cl_mem input_left  = clCreateBuffer(context, CL_MEM_READ_ONLY,  sizeof(float) * (DATA_SIZE - 1), NULL, &err);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clCreateBuffer [ %s ]\n", err_str);
#endif
    cl_mem input_right = clCreateBuffer(context, CL_MEM_READ_ONLY,  sizeof(float) * (DATA_SIZE - 1), NULL, &err);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clCreateBuffer [ %s ]\n", err_str);
#endif
    cl_mem output      = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * (DATA_SIZE - 1), NULL, &err);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clCreateBuffer [ %s ]\n", err_str);
#endif

    err = clEnqueueWriteBuffer(command_queue, input_left,  CL_TRUE, 0, sizeof(float) * (DATA_SIZE - 1), in_data,     0, NULL, NULL);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clEnqueueWriteBuffer [ %s ]\n", err_str);
#endif
    err = clEnqueueWriteBuffer(command_queue, input_right, CL_TRUE, 0, sizeof(float) * (DATA_SIZE - 1), in_data + 1, 0, NULL, NULL);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clEnqueueWriteBuffer [ %s ]\n", err_str);
#endif

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_left);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clSetKernelArg [ %s ]\n", err_str);
#endif
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &input_right);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clSetKernelArg [ %s ]\n", err_str);
#endif
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &output);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clSetKernelArg [ %s ]\n", err_str);
#endif



    /*
     * вычисление кол-ва групп и размара каждой группы
     */

    size_t div = (DATA_SIZE - 1) / V_LEN;
    size_t global_work_size = ((DATA_SIZE - 1) % V_LEN == 0) ? div : div + 1;

//    Необходим алгоритм вычисления размера локальной группы.
//    Он должен быть кратен 64.
    size_t local_work_size = 1;
    cl_event myEvent;
    clock_gettime(CLOCK_ID, &start);


    /*
     * выполнение ядра
     */

    err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, &myEvent);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clEnqueueNDRangeKernel [ %s ]\n", err_str);
#endif
    clFinish(command_queue);




    /*
     * вычисление времени выполнения
     */

    clock_gettime(CLOCK_ID, &stop);
    dsec = stop.tv_sec - start.tv_sec;
    dnsec = stop.tv_nsec - start.tv_nsec;

    cl_ulong start_time, end_time, queued_time, submit_time;
    err = clGetEventProfilingInfo(myEvent, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &queued_time, NULL);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clGetEventProfilingInfo [ %s ]\n", err_str);
#endif

    err = clGetEventProfilingInfo(myEvent, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &submit_time, NULL);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clGetEventProfilingInfo [ %s ]\n", err_str);
#endif

    err = clGetEventProfilingInfo(myEvent, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start_time, NULL);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clGetEventProfilingInfo [ %s ]\n", err_str);
#endif

    err = clGetEventProfilingInfo(myEvent, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end_time, NULL);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clGetEventProfilingInfo [ %s ]\n", err_str);
#endif

    printf("GPU SSE: %f sec { sec: %ld, nsec: %ld }\n", dsec + (dnsec / 1000000000.0), dsec, dnsec);

    printf("\nqueued time:\t%lld nsec\n", queued_time);
    printf("submit time:\t%lld nsec\n", submit_time);
    printf("start time:\t%lld nsec\n", start_time);
    printf("end time:\t%lld nsec\n\n", end_time);

    //cl_ulong kernel_queued_time = submit_time - queued_time;
    printf("kernel queued time:\t%lld nsec\n", submit_time - queued_time);

    //cl_ulong kernel_sbmit_time = start_time - submit_time;
    printf("kernel submit time:\t%lld nsec\n", start_time - submit_time);

    //cl_ulong kernel_exec_time = end_time - start_time;
    printf("kernel exec time:\t%lld nsec\n", end_time - start_time);

    printf("-------------------------------------------------\n");
    //cl_ulong kernel_total_time = end_time - queued_time;
    printf("kernel total time:\t%lld nsec\n\n", end_time - queued_time);



    clEnqueueReadBuffer(command_queue, output, CL_TRUE, 0, sizeof(float) * (DATA_SIZE - 1), out_data, 0, NULL, NULL);

    printf("global work size: %d\tlocal work size: %d\n\n", global_work_size, local_work_size);

    printf("output: ");
    for(i = 0; i < DATA_SIZE - 1; i++)
    {
        printf("%f ", out_data[i]);
    }
    printf("\n");

    free(out_data);
    free(in_data);

    clReleaseMemObject(output);
    clReleaseMemObject(input_right);
    clReleaseMemObject(input_left);

    clReleaseKernel(kernel);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);

    return 0;
}
