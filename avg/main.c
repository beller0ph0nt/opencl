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
#define DATA_SIZE       10000000

#define KERNEL_SRC      "average.cl"
#define BUILD_OPTIONS   "-I ./"

#define MAX_PLATFORMS   100
#define MAX_DEVICES     100

//struct dev_prop
//{
//    size_t  max_param_size;
//    int ratio;
//};

cl_uint total_devices_count = 0;
cl_uint *devices_on_platform = NULL;
cl_device_id **devices = NULL;
//struct dev_prop **devices_prop;

cl_uint platforms_count = 0;
cl_platform_id *platforms = NULL;

cl_context **contexts = NULL;
cl_command_queue **cmd_queues = NULL;



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
//    free_ptr_2d((void **) devices_prop, platforms_count);
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
//            devices_prop = malloc(sizeof(*devices_prop) * platforms_count);
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
                    total_devices_count += devices_on_platform[i];

                    devices[i] = malloc(sizeof(**devices) * devices_on_platform[i]);
//                    devices_prop[i] = malloc(sizeof(**devices_prop) * devices_on_platform[i]);
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
//                            clGetDeviceInfo(devices[i][j],
//                                           CL_DEVICE_MAX_PARAMETER_SIZE,
//                                           sizeof(devices_prop[i][j].max_param_size),
//                                           &devices_prop[i][j].max_param_size,
//                                           NULL);

                            // !!! на базе считанных параметров необходимо вычислить коэффициент от 1 до 100 !!!
                            // !!! пропорционально данному коэффициенту модули будут разбивать данные для параллельного вычисления !!!
#ifdef DEBUG
                            printf("platform: %d\t cmd queue: %d\n", i, j);
#endif
                            contexts[i][j] = clCreateContext(properties, 1, &devices[i][j], NULL, NULL, err);
                            if (err != NULL)
                            {
                                cmd_queues[i][j] = clCreateCommandQueue(contexts[i][j], devices[i][j], CL_QUEUE_PROFILING_ENABLE, err);
                                if (err == NULL)
                                {
#ifdef DEBUG
                                    printf("clCreateCommandQueue [ ERROR ]\n");
#endif
                                    ret = 6;
                                    break;
                                }
                            }
                            else
                            {
#ifdef DEBUG
                                printf("clCreateContext [ ERROR ]\n");
#endif
                                ret = 5;
                                break;
                            }
                        }
                    }
                    else
                    {
#ifdef DEBUG
                        printf("clGetDeviceIDs [ ERROR ]\n");
#endif
                        ret = 4;
                    }
                }
                else
                {
#ifdef DEBUG
                    printf("clGetDeviceIDs [ ERROR ]\n");
#endif
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
#ifdef DEBUG
        printf("clGetPlatformIDs [ ERROR ]\n");
#endif
        ret = 1;
    }

    // этот блок необходимо вынести во внешнюю функцию
    // после неудачной инициализации, она должна очищать память
//    if (ret != 0)
//    {
//        clear();
//    }

    return ret;
}





cl_program **avg_programs = NULL;
cl_kernel **avg_kernels = NULL;

int avg_init()
{
    int ret = 0;

    int len;
    if (kernel_len(KERNEL_SRC, &len) == 0)
    {
        char **kernel_src = NULL;
        kernel_src = malloc(sizeof(*kernel_src));
        kernel_src[0] = malloc(sizeof(**kernel_src) * (len + 1));
        kernel_src[0][len] = 0;
        if (kernel_read(KERNEL_SRC, len, kernel_src[0]) == 0)
        {
#ifdef DEBUG
            char err_str[MAX_STR_ERR_LEN];
#endif
            int i;
            cl_int err;

            avg_programs = malloc(sizeof(*avg_programs) * platforms_count);
            avg_kernels = malloc(sizeof(*avg_kernels) * platforms_count);
            for (i = 0; i < platforms_count; i++)
            {
                avg_programs[i] = malloc(sizeof(**avg_programs) * devices_on_platform[i]);
                avg_kernels[i] = malloc(sizeof(**avg_kernels) * devices_on_platform[i]);

                int j;
                for (j = 0; j < devices_on_platform[i]; j++)
                {
                    //cl_program program = clCreateProgramWithSource(contexts[i][j], 1, (const char **) kernel_src, NULL, &err);
                    avg_programs[i][j] = clCreateProgramWithSource(contexts[i][j], 1, (const char **) kernel_src, NULL, &err);
                    if (err == CL_SUCCESS)
                    {
                        err = clBuildProgram(avg_programs[i][j], 0, NULL, BUILD_OPTIONS, NULL, NULL);
                        if (err == CL_SUCCESS)
                        {
                            avg_kernels[i][j] = clCreateKernel(avg_programs[i][j], AVG_PROG_NAME_STR, &err);
                            if (err != CL_SUCCESS)
                            {
                                ret = 5;
                            }
                        }
                        else
                        {
                            ret = 4;
                            printf("Error building program\n");

                            char buffer[4096];
                            size_t length;
                            clGetProgramBuildInfo(avg_programs[i][j], devices[i][j], CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);

                            printf("%s\n", buffer);
                        }
                    }
                    else
                    {
                        ret = 3;
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
#ifdef DEBUG
            printf("kernel_read [ ERROR ]\n");
#endif
            ret = 2;
        }

        free_ptr_2d((void **) kernel_src, 1);
    }
    else
    {
#ifdef DEBUG
        printf("kernel_len [ ERROR ]\n");
#endif
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
            clReleaseProgram(avg_programs[i][j]);
        }
    }

    free_ptr_2d((void **) avg_kernels, platforms_count);
    free_ptr_2d((void **) avg_programs, platforms_count);
}

int avg_calc(const double *in, const unsigned long int len)
{
    int ret = 0;

    /*

    struct par_info {       // !!! необходимо в соответствии с текущей структурой подправить алгоримт
        double *start;
        unsigned long int len;
        cl_mem mem;
    };

    struct block_info {
        struct par_info in_left;
        struct par_info in_right;
        struct par_info out;

        double *start;
        unsigned long int len;

        cl_mem in_left;
        cl_mem in_right;
        cl_mem output;

    };

    struct block_info **blocks = NULL;

    // !!! вычисление длинны блока для устройства
    double *cur_pointer = in;
    cl_uint cur_device_index = 1;
    unsigned long int remain_len = len;
    unsigned long int block_len = len / total_devices_count;


    // !!! необходимо для каждого ядра задать свой набор параметров, между которыми
    // !!! пропорционально разделить данные...


    blocks = malloc(sizeof(*blocks) * platforms_count);

    int i;
    for (i = 0; i < platforms_count; i++)
    {
        blocks[i] = malloc(sizeof(**blocks) * devices_on_platform[i]);
        int j;
        for (j = 0; j < devices_on_platform[i]; j++)
        {
            if (cur_device_index == total_devices_count)
            {
                blocks[i][j].start = cur_pointer;
                blocks[i][j].len = remain_len;

                remain_len = 0;
                cur_pointer = NULL;
            }
            else
            {
                blocks[i][j].start = cur_pointer;
                blocks[i][j].len = block_len;

                remain_len -= block_len;
                cur_pointer += block_len;
            }




            //    настройка переменных

            blocks[i][j].in_left = clCreateBuffer(contexts[i][j], CL_MEM_READ_ONLY,  sizeof(*in) * (blocks[i][j].len - 1), NULL, &err);     // !!! необходимо условие последнего блока. для всех остальных блоков, кроме последнего длинна будет равна blocks[i][j].len
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clCreateBuffer [ %s ]\n", err_str);
#endif
            err = clSetKernelArg(avg_kernels[i][j], 0, sizeof(blocks[i][j].in_left), &blocks[i][j].in_left);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clSetKernelArg [ %s ]\n", err_str);
#endif


            blocks[i][j].in_right = clCreateBuffer(contexts[i][j], CL_MEM_READ_ONLY,  sizeof(*in) * (blocks[i][j].len - 1), NULL, &err);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clCreateBuffer [ %s ]\n", err_str);
#endif
            err = clSetKernelArg(avg_kernels[i][j], 1, sizeof(blocks[i][j].in_right), &blocks[i][j].in_right);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clSetKernelArg [ %s ]\n", err_str);
#endif


            blocks[i][j].output      = clCreateBuffer(contexts[i][j], CL_MEM_WRITE_ONLY, sizeof(*in) * (blocks[i][j].len - 1), NULL, &err);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clCreateBuffer [ %s ]\n", err_str);
#endif
            err = clSetKernelArg(avg_kernels[i][j], 2, sizeof(blocks[i][j].output), &blocks[i][j].output);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clSetKernelArg [ %s ]\n", err_str);
#endif



            //     заполнение видео-памяти

            if (remain_len != 0)
            {
                err = clEnqueueWriteBuffer(command_queue[i][j], blocks[i][j].in_left, CL_TRUE, 0, sizeof(double) * (blocks[i][j].len),     blocks[i][j].start, 0, NULL, NULL);
            }
            else
            {
                err = clEnqueueWriteBuffer(command_queue[i][j], blocks[i][j].in_left, CL_TRUE, 0, sizeof(double) * (blocks[i][j].len - 1), blocks[i][j].start, 0, NULL, NULL);
            }
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clEnqueueWriteBuffer [ %s ]\n", err_str);
#endif
            err = clEnqueueWriteBuffer(command_queue, in_right, CL_TRUE, 0, sizeof(double) * (DATA_SIZE - 1), in_data + 1, 0, NULL, NULL);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clEnqueueWriteBuffer [ %s ]\n", err_str);
#endif
        }
    }



*/


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


    double *in_data = malloc(sizeof(*in_data) * DATA_SIZE);
    double *out_data = malloc(sizeof(*out_data) * (DATA_SIZE - 1));

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

    cl_kernel kernel = clCreateKernel(program, TO_STR(AVG_PROG_NAME), &err);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clCreateKernel [ %s ]\n", err_str);
#endif

    clReleaseProgram(program);





    /*
     * настройка переменных
     */

    cl_mem input_left  = clCreateBuffer(context, CL_MEM_READ_ONLY,  sizeof(double) * (DATA_SIZE - 1), NULL, &err);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clCreateBuffer [ %s ]\n", err_str);
#endif
    err = clSetKernelArg(kernel, 0, sizeof(input_left), &input_left);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clSetKernelArg [ %s ]\n", err_str);
#endif


    cl_mem input_right = clCreateBuffer(context, CL_MEM_READ_ONLY,  sizeof(double) * (DATA_SIZE - 1), NULL, &err);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clCreateBuffer [ %s ]\n", err_str);
#endif
    err = clSetKernelArg(kernel, 1, sizeof(input_right), &input_right);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clSetKernelArg [ %s ]\n", err_str);
#endif


    cl_mem output      = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(double) * (DATA_SIZE - 1), NULL, &err);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clCreateBuffer [ %s ]\n", err_str);
#endif
    err = clSetKernelArg(kernel, 2, sizeof(output), &output);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clSetKernelArg [ %s ]\n", err_str);
#endif

    /*
     * заполнение видео-памяти
     */

    err = clEnqueueWriteBuffer(command_queue, input_left,  CL_TRUE, 0, sizeof(double) * (DATA_SIZE - 1), in_data,     0, NULL, NULL);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clEnqueueWriteBuffer [ %s ]\n", err_str);
#endif
    err = clEnqueueWriteBuffer(command_queue, input_right, CL_TRUE, 0, sizeof(double) * (DATA_SIZE - 1), in_data + 1, 0, NULL, NULL);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clEnqueueWriteBuffer [ %s ]\n", err_str);
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

    printf("kernel queued time:\t%lld nsec\n", submit_time - queued_time);
    printf("kernel submit time:\t%lld nsec\n", start_time - submit_time);
    printf("kernel exec time:\t%lld nsec\n", end_time - start_time);
    printf("-------------------------------------------------\n");
    printf("kernel total time:\t%lld nsec\n\n", end_time - queued_time);



    clEnqueueReadBuffer(command_queue, output, CL_TRUE, 0, sizeof(double) * (DATA_SIZE - 1), out_data, 0, NULL, NULL);

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
