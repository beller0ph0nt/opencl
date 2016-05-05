#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <time.h>
#include <stdio.h>
#include <CL/cl.h>
#include <stdlib.h>

//#include "err.h"
//#include "file.h"
//#include "free.h"
//#include "params.h"

#include "kernel.h"
#include "context.h"
#include "program.h"
#include "kernel_avg/kernel.h"

//#define CLOCK_ID        CLOCK_REALTIME  // CLOCK_PROCESS_CPUTIME_ID    //
//#define KERNELS_COUNT   1

//#define DATA_SIZE       16384     // V_LEN * max_param_size
#define DATA_SIZE       10000000

//#define KERNEL_SRC      "average.cl"
//#define BUILD_OPTIONS   "-I ./"

//#define MAX_PLATFORMS   100
//#define MAX_DEVICES     100

/*

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
    int plat, dev;
    for (plat = 0; plat < platforms_count; plat++)
    {
        for (dev = 0; dev < devices_on_platform[plat]; dev++)
        {
            clReleaseCommandQueue(cmd_queues[plat][dev]);
            clReleaseContext(contexts[plat][dev]);
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

#ifdef DEBUG
    char err_str[MAX_STR_ERR_LEN];
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


            int plat;
            for (plat = 0; plat < platforms_count; plat++)
            {
                if (clGetDeviceIDs(platforms[plat], dev_type, 0, NULL, &devices_on_platform[plat]) == CL_SUCCESS)
                {
#ifdef DEBUG
                    printf("platform: %d\t devices: %d\n", plat, devices_on_platform[plat]);
#endif
                    total_devices_count += devices_on_platform[plat];

                    devices[plat] = malloc(sizeof(**devices) * devices_on_platform[plat]);
//                    devices_prop[i] = malloc(sizeof(**devices_prop) * devices_on_platform[i]);
                    contexts[plat] = malloc(sizeof(**contexts) * devices_on_platform[plat]);
                    cmd_queues[plat] = malloc(sizeof(**cmd_queues) * devices_on_platform[plat]);

                    cl_int *err = NULL;

                    if (clGetDeviceIDs(platforms[plat], dev_type, devices_on_platform[plat], devices[plat], NULL) == CL_SUCCESS)
                    {
                        cl_context_properties properties[3] = {
                            CL_CONTEXT_PLATFORM,
                            (cl_context_properties) platforms[plat],
                            0
                        };

                        int dev;
                        for (dev = 0; dev < devices_on_platform[plat]; dev++)
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
                            printf("plat: %d\t dev: %d\n", plat, dev);
#endif
                            contexts[plat][dev] = clCreateContext(properties, 1, &devices[plat][dev], NULL, NULL, err);
                            if (err == NULL)
                            {
                                cmd_queues[plat][dev] = clCreateCommandQueue(contexts[plat][dev], devices[plat][dev], CL_QUEUE_PROFILING_ENABLE, err);
                                if (err != NULL)
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
                                err_to_str((*err), err_str);
                                printf("clCreateContext [ %s ]\n", err_str);
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

struct kernel_prop
{
    size_t  pref_work_group_size_mult;
};

struct kernel_prop **avg_kernel_prop = NULL;

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
            avg_programs = malloc(sizeof(*avg_programs) * platforms_count);
            avg_kernels = malloc(sizeof(*avg_kernels) * platforms_count);
            avg_kernel_prop = malloc(sizeof(*avg_kernel_prop) * platforms_count);

            int plat;
            cl_int err;
            for (plat = 0; plat < platforms_count; plat++)
            {
                avg_programs[plat] = malloc(sizeof(**avg_programs) * devices_on_platform[plat]);
                avg_kernels[plat] = malloc(sizeof(**avg_kernels) * devices_on_platform[plat]);
                avg_kernel_prop[plat] = malloc(sizeof(**avg_kernel_prop) * devices_on_platform[plat]);

                int dev;
                for (dev = 0; dev < devices_on_platform[plat]; dev++)
                {
                    avg_programs[plat][dev] = clCreateProgramWithSource(contexts[plat][dev],
                                                                        1,
                                                                        (const char **) kernel_src,
                                                                        NULL,
                                                                        &err);
                    if (err == CL_SUCCESS)
                    {
                        err = clBuildProgram(avg_programs[plat][dev],
                                             0,
                                             NULL,
                                             BUILD_OPTIONS,
                                             NULL,
                                             NULL);
                        if (err == CL_SUCCESS)
                        {
                            avg_kernels[plat][dev] = clCreateKernel(avg_programs[plat][dev],
                                                                    AVG_PROG_NAME_STR,
                                                                    &err);
                            if (err == CL_SUCCESS)
                            {
                                 err = clGetKernelWorkGroupInfo(avg_kernels[plat][dev],
                                                                devices[plat][dev],
                                                                CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,
                                                                sizeof(avg_kernel_prop[plat][dev].pref_work_group_size_mult),
                                                                &avg_kernel_prop[plat][dev].pref_work_group_size_mult,
                                                                NULL);
                                 if (err != CL_SUCCESS)
                                 {
                                     ret = 6;
                                 }
                            }
                            else
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
                            clGetProgramBuildInfo(avg_programs[plat][dev],
                                                  devices[plat][dev],
                                                  CL_PROGRAM_BUILD_LOG,
                                                  sizeof(buffer),
                                                  buffer,
                                                  &length);

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

    free_ptr_2d((void **) avg_kernel_prop, platforms_count);
    free_ptr_2d((void **) avg_kernels, platforms_count);
    free_ptr_2d((void **) avg_programs, platforms_count);
}

int avg_calc(const double *in, const unsigned long int len)
{
    int ret = 0;

    struct timespec start, stop;
    long dsec, dnsec;





    struct par_info {
        double* start;
        unsigned long int len;
        cl_mem mem;
    };

    struct work_size {
        size_t global;
        size_t local;
    };

    struct work_info {
        struct work_size size;
    };

    struct time_info {
        cl_ulong queued;
        cl_ulong submit;
        cl_ulong start;
        cl_ulong end;
    };

    struct block_info {
        struct par_info left;
        struct par_info right;
        struct par_info out;
        struct work_info work;
        cl_event event;
        struct time_info time;
    };

    struct block_info **blocks = NULL;


    double *cur_pointer = (double *) in;

    unsigned long int remain_len = len;
    unsigned long int block_len = len / total_devices_count;

    blocks = malloc(sizeof(*blocks) * platforms_count);

    cl_int err;
#ifdef DEBUG
    char err_str[MAX_STR_ERR_LEN];
#endif

    int plat, dev;
    cl_uint cur_device_index = 1;
    for (plat = 0; plat < platforms_count; plat++)
    {
        blocks[plat] = malloc(sizeof(**blocks) * devices_on_platform[plat]);


        for (dev = 0; dev < devices_on_platform[plat]; dev++, cur_device_index++)
        {
            if (cur_device_index != total_devices_count)
            {
                blocks[plat][dev].left.len = block_len;
                blocks[plat][dev].left.start = cur_pointer;

                blocks[plat][dev].right.len = block_len;
                blocks[plat][dev].right.start = cur_pointer + 1;

                blocks[plat][dev].out.len = block_len;
                blocks[plat][dev].out.start = malloc(sizeof(*blocks[plat][dev].out.start) * blocks[plat][dev].out.len);

                remain_len -= block_len;
                cur_pointer += block_len;
            }
            else
            {
                blocks[plat][dev].left.len = remain_len - 1;
                blocks[plat][dev].left.start = cur_pointer;

                blocks[plat][dev].right.len = remain_len - 1;
                blocks[plat][dev].right.start = cur_pointer + 1;

                blocks[plat][dev].out.len = remain_len - 1;
                blocks[plat][dev].out.start = malloc(sizeof(*blocks[plat][dev].out.start) * blocks[plat][dev].out.len);

                remain_len = 0;
                cur_pointer = NULL;
            }


            //    настройка переменных

            blocks[plat][dev].left.mem = clCreateBuffer(contexts[plat][dev],
                                                        CL_MEM_READ_ONLY,
                                                        sizeof(*in) * blocks[plat][dev].left.len,
                                                        NULL,
                                                        &err);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clCreateBuffer [ %s ]\n", err_str);
#endif

            err = clSetKernelArg(avg_kernels[plat][dev],
                                 0,
                                 sizeof(blocks[plat][dev].left.mem),
                                 &blocks[plat][dev].left.mem);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clSetKernelArg [ %s ]\n", err_str);
#endif


            blocks[plat][dev].right.mem = clCreateBuffer(contexts[plat][dev],
                                                         CL_MEM_READ_ONLY,
                                                         sizeof(*in) * blocks[plat][dev].right.len,
                                                         NULL,
                                                         &err);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clCreateBuffer [ %s ]\n", err_str);
#endif

            err = clSetKernelArg(avg_kernels[plat][dev],
                                 1,
                                 sizeof(blocks[plat][dev].right.mem),
                                 &blocks[plat][dev].right.mem);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clSetKernelArg [ %s ]\n", err_str);
#endif


            blocks[plat][dev].out.mem = clCreateBuffer(contexts[plat][dev],
                                                       CL_MEM_WRITE_ONLY,
                                                       sizeof(*in) * blocks[plat][dev].out.len,
                                                       NULL,
                                                       &err);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clCreateBuffer [ %s ]\n", err_str);
#endif

            err = clSetKernelArg(avg_kernels[plat][dev],
                                 2,
                                 sizeof(blocks[plat][dev].out.mem),
                                 &blocks[plat][dev].out.mem);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clSetKernelArg [ %s ]\n", err_str);
#endif



            //     заполнение видео-памяти

            err = clEnqueueWriteBuffer(cmd_queues[plat][dev],
                                       blocks[plat][dev].left.mem,
                                       CL_TRUE,
                                       0,
                                       sizeof(*in) * blocks[plat][dev].left.len,
                                       blocks[plat][dev].left.start,
                                       0,
                                       NULL,
                                       NULL);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clEnqueueWriteBuffer [ %s ]\n", err_str);
#endif

            err = clEnqueueWriteBuffer(cmd_queues[plat][dev],
                                       blocks[plat][dev].right.mem,
                                       CL_TRUE,
                                       0,
                                       sizeof(*in) * blocks[plat][dev].right.len,
                                       blocks[plat][dev].right.start,
                                       0,
                                       NULL,
                                       NULL);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clEnqueueWriteBuffer [ %s ]\n", err_str);
#endif

            size_t div = blocks[plat][dev].out.len / V_LEN;
            blocks[plat][dev].work.size.global = (blocks[plat][dev].out.len % V_LEN == 0) ? div : div + 1;

            div = blocks[plat][dev].work.size.global / avg_kernel_prop[plat][dev].pref_work_group_size_mult;
            blocks[plat][dev].work.size.global = ((blocks[plat][dev].work.size.global % avg_kernel_prop[plat][dev].pref_work_group_size_mult == 0) ? div : div + 1) * avg_kernel_prop[plat][dev].pref_work_group_size_mult;

            blocks[plat][dev].work.size.local = avg_kernel_prop[plat][dev].pref_work_group_size_mult;
        }
    }


    clock_gettime(CLOCK_ID, &start);

    for (plat = 0; plat < platforms_count; plat++)
    {
        for (dev = 0; dev < devices_on_platform[plat]; dev++)
        {
            err = clEnqueueNDRangeKernel(cmd_queues[plat][dev],
                                         avg_kernels[plat][dev],
                                         1,
                                         NULL,
                                         &blocks[plat][dev].work.size.global,
                                         &blocks[plat][dev].work.size.local,
                                         0,
                                         NULL,
                                         &blocks[plat][dev].event);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clEnqueueNDRangeKernel [ %s ]\n", err_str);
#endif
        }
    }


    for (plat = 0; plat < platforms_count; plat++)
    {
        for (dev = 0; dev < devices_on_platform[plat]; dev++)
        {
            clFinish(cmd_queues[plat][dev]);
        }
    }

    clock_gettime(CLOCK_ID, &stop);
    dsec = stop.tv_sec - start.tv_sec;
    dnsec = stop.tv_nsec - start.tv_nsec;

    printf("\nGPU: %f sec { sec: %ld, nsec: %ld }\n\n", dsec + (dnsec / 1000000000.0), dsec, dnsec);



    cl_ulong total_queued_time = 0;
    cl_ulong total_submit_time = 0;
    cl_ulong total_exec_time = 0;
    cl_ulong total_time = 0;


    for (plat = 0; plat < platforms_count; plat++)
    {
        for (dev = 0; dev < devices_on_platform[plat]; dev++)
        {
            err = clGetEventProfilingInfo(blocks[plat][dev].event,
                                          CL_PROFILING_COMMAND_QUEUED,
                                          sizeof(blocks[plat][dev].time.queued),
                                          &blocks[plat][dev].time.queued,
                                          NULL);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clGetEventProfilingInfo [ %s ]\n", err_str);
#endif

            err = clGetEventProfilingInfo(blocks[plat][dev].event,
                                          CL_PROFILING_COMMAND_SUBMIT,
                                          sizeof(blocks[plat][dev].time.submit),
                                          &blocks[plat][dev].time.submit,
                                          NULL);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clGetEventProfilingInfo [ %s ]\n", err_str);
#endif

            err = clGetEventProfilingInfo(blocks[plat][dev].event,
                                          CL_PROFILING_COMMAND_START,
                                          sizeof(blocks[plat][dev].time.start),
                                          &blocks[plat][dev].time.start,
                                          NULL);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clGetEventProfilingInfo [ %s ]\n", err_str);
#endif

            err = clGetEventProfilingInfo(blocks[plat][dev].event,
                                          CL_PROFILING_COMMAND_END,
                                          sizeof(blocks[plat][dev].time.end),
                                          &blocks[plat][dev].time.end,
                                          NULL);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clGetEventProfilingInfo [ %s ]\n", err_str);
#endif

            printf("\nqueued time:\t%lld nsec\n", blocks[plat][dev].time.queued);
            printf("submit time:\t%lld nsec\n", blocks[plat][dev].time.submit);
            printf("start time:\t%lld nsec\n", blocks[plat][dev].time.start);
            printf("end time:\t%lld nsec\n\n", blocks[plat][dev].time.end);

            printf("kernel queued time:\t%lld nsec\n", blocks[plat][dev].time.submit - blocks[plat][dev].time.queued);
            printf("kernel submit time:\t%lld nsec\n", blocks[plat][dev].time.start - blocks[plat][dev].time.submit);
            printf("kernel exec time:\t%lld nsec\n", blocks[plat][dev].time.end - blocks[plat][dev].time.start);
            printf("-------------------------------------------------\n");
            printf("kernel total time:\t%lld nsec\n\n", blocks[plat][dev].time.end - blocks[plat][dev].time.queued);

            total_queued_time += blocks[plat][dev].time.submit - blocks[plat][dev].time.queued;
            total_submit_time += blocks[plat][dev].time.start - blocks[plat][dev].time.submit;
            total_exec_time += blocks[plat][dev].time.end - blocks[plat][dev].time.start;
            total_time += blocks[plat][dev].time.end - blocks[plat][dev].time.queued;
        }
    }


    printf("total queued time:\t%lld nsec\n", total_queued_time);
    printf("total submit time:\t%lld nsec\n", total_submit_time);
    printf("total exec time:\t%lld nsec\n", total_exec_time);
    printf("-------------------------------------------------\n");
    printf("total time:\t%lld nsec\n\n", total_time);

    for (plat = 0; plat < platforms_count; plat++)
    {
        for (dev = 0; dev < devices_on_platform[plat]; dev++)
        {


            err = clEnqueueReadBuffer(cmd_queues[plat][dev],
                                      blocks[plat][dev].out.mem,
                                      CL_TRUE,
                                      0,
                                      sizeof(*blocks[plat][dev].out.start) * blocks[plat][dev].out.len,
                                      blocks[plat][dev].out.start,
                                      0,
                                      NULL,
                                      NULL);
#ifdef DEBUG
            err_to_str(err, err_str);
            printf("clEnqueueReadBuffer [ %s ]\n", err_str);
#endif

//            printf("output: ");
//            unsigned long i;
//            for(i = 0; i < blocks[plat][dev].out.len; i++)
//            {
//                printf("%f ", blocks[plat][dev].out.start[i]);
//            }
//            printf("\n");


            free_ptr_1d(blocks[plat][dev].out.start);

            clReleaseMemObject(blocks[plat][dev].left.mem);
            clReleaseMemObject(blocks[plat][dev].right.mem);
            clReleaseMemObject(blocks[plat][dev].out.mem);
        }
    }


    return ret;
}



void cpu_test()
{
//    clock_gettime(CLOCK_ID, &start);

//    for (i = 0; i < DATA_SIZE - 1; i++)
//    {
//        out_data[i] = (in_data[i] + in_data[i + 1]) / 2.0;
//    }

//    clock_gettime(CLOCK_ID, &stop);
//    dsec = stop.tv_sec - start.tv_sec;
//    dnsec = stop.tv_nsec - start.tv_nsec;
//    printf("CPU: %f sec { sec: %ld, nsec: %ld }\n", dsec + (dnsec / 1000000000.0), dsec, dnsec);
}

*/

int main()
{
    double *in_data = malloc(sizeof(*in_data) * DATA_SIZE);
    double *out_data = malloc(sizeof(*out_data) * (DATA_SIZE - 1));

    long int i;
    for (i = 0; i < DATA_SIZE; i++)
    {
        in_data[i] = i;
    }


    context_t* context = context_create(CL_DEVICE_TYPE_GPU);
    if (context != NULL)
    {
        program_t* prog = program_create_src(context, "kernel/avg.cl", "avg");
        if (prog != NULL)
        {
            kernel_t* kern = kernel_create(context, prog);
            if (kern != NULL)
            {
                struct avg_params_t params;
                params.in = in_data;
                params.in_len = DATA_SIZE;
                params.out = out_data;
                params.out_len = DATA_SIZE - 1;

                kernel_avg_calc(context, kern, &params);

                    for (i = 0; i < DATA_SIZE - 1; i++)
                    {
                        printf("%f ", params.out[i]);
                    }
                    printf("\n");

                kernel_clear(context, kern);
            }

            program_clear(context, prog);
        }

        context_clear(context);
    }



/*
    if (init(CL_DEVICE_TYPE_GPU) == 0)
    {
        if (avg_init() == 0)
        {
            avg_calc(in_data, DATA_SIZE);
        }

        avg_clear();
    }

    clear();
*/



/*

#ifdef DEBUG
    char err_str[MAX_STR_ERR_LEN];
#endif
    struct timespec start, stop;
    long dsec, dnsec;

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




    // процесс инициализации ядра

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





     // настройка переменных

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

     // заполнение видео-памяти

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









     // вычисление кол-ва групп и размера каждой группы

    size_t group_size_mult = 64;

    size_t div = (DATA_SIZE - 1) / V_LEN;
    size_t global_work_size = ((DATA_SIZE - 1) % V_LEN == 0) ? div : div + 1;

    div = global_work_size / group_size_mult;
    global_work_size = ((global_work_size % group_size_mult == 0) ? div : div + 1) * group_size_mult;

    size_t local_work_size = group_size_mult;

    cl_event myEvent;
    clock_gettime(CLOCK_ID, &start);


     // выполнение ядра

    err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, &myEvent);
#ifdef DEBUG
    err_to_str(err, err_str);
    printf("clEnqueueNDRangeKernel [ %s ]\n", err_str);
#endif
    clFinish(command_queue);




     // вычисление времени выполнения

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

*/

    return 0;
}
