#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <time.h>
#include <stdio.h>
#include <CL/cl.h>

#define DATA_SIZE 1000000

const char * KernelSource =
"__kernel void hello(__global float * input, __global float * output)   \n"\
"{                                                                      \n"\
"  size_t id = get_global_id(0);                                        \n"\
"  output[id] = (input[id] * 2 / 2) * 3 / 3;                            \n"\
"}                                                                      \n"\
"                                                                       \n";

int main(void)
{
    cl_context context;
    cl_context_properties properties[3];
    cl_kernel kernel;
    cl_command_queue command_queue;
    cl_program program;
    cl_int err;
    cl_uint num_of_platforms = 0;
    cl_platform_id platform_id;
    cl_device_id device_id;
    cl_uint num_of_devices = 0;
    cl_mem input, output;
    size_t global;
    struct timespec start, stop;
    long dsec, dnsec;

    float inputData[DATA_SIZE] = { 0 };
    float results[DATA_SIZE] = { 0 };

    int i;

    for (i = 0; i < DATA_SIZE; i++)
    {
        inputData[i] = i + 1;
    }

    //CLOCK_PROCESS_CPUTIME_ID
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);  //    clock_gettime(CLOCK_REALTIME, &start);
    for (i = 0; i < DATA_SIZE; i++)
    {
        results[i] = (inputData[i] * 2 / 2) * 3 / 3;
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);   //clock_gettime(CLOCK_REALTIME, &stop);
    dsec = stop.tv_sec - start.tv_sec;
    dnsec = stop.tv_nsec - start.tv_nsec;
    printf("CPU: %f sec { sec: %ld, nsec: %ld }\n", dsec + (dnsec / 1000000000.0), dsec, dnsec);

    for (i = 0; i < DATA_SIZE; i++)
    {
        inputData[i] = i + 1;
        results[i] = 0;
    }

    // retreives a list of platforms available
    if (clGetPlatformIDs(1, &platform_id, &num_of_platforms) != CL_SUCCESS)
    {
        printf("Unable to get platform_id\n");
        return 1;
    }

    // try to get a supported GPU device
    if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 1, &device_id, &num_of_devices) != CL_SUCCESS)
    {
        printf("Unable to get device_id\n");
        return 1;
    }

    // context properties list - must be terminated with 0
    properties[0] = CL_CONTEXT_PLATFORM;
    properties[1] = (cl_context_properties) platform_id;
    properties[2] = 0;

    // create a context with the GPU device
    context = clCreateContext(properties,1,&device_id,NULL,NULL,&err);

    // create command queue using the context and device
    command_queue = clCreateCommandQueue(context, device_id, (cl_command_queue_properties) 0, &err);
    // create a program from the kernel source code
    program = clCreateProgramWithSource(context, 1, (const char **) &KernelSource, NULL, &err);

    // compile the program
    if (clBuildProgram(program, 0, NULL, NULL, NULL, NULL) != CL_SUCCESS)
    {
        printf("Error building program\n");
        return 1;
    }

    // specify which kernel from the program to execute
    kernel = clCreateKernel(program, "hello", &err);

    // create buffers for the input and ouput
    input = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * DATA_SIZE, NULL, NULL);
    output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * DATA_SIZE, NULL, NULL);

    // load data into the input buffer
    clEnqueueWriteBuffer(command_queue, input, CL_TRUE, 0, sizeof(float) * DATA_SIZE, inputData, 0, NULL, NULL);

    // set the argument list for the kernel command
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &output);
    global = DATA_SIZE;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);    //clock_gettime(CLOCK_REALTIME, &start);
    // enqueue the kernel command for execution
    clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global, NULL, 0, NULL, NULL);
    clFinish(command_queue);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);   //clock_gettime(CLOCK_REALTIME, &stop);
    dsec = stop.tv_sec - start.tv_sec;
    dnsec = stop.tv_nsec - start.tv_nsec;
    printf("CPU SSE: %f sec { sec: %ld, nsec: %ld }\n", dsec + (dnsec / 1000000000.0), dsec, dnsec);

    // copy the results from out of the output buffer
    clEnqueueReadBuffer(command_queue, output, CL_TRUE, 0, sizeof(float) * DATA_SIZE, results, 0, NULL, NULL);

    // print the results
//    printf("output: ");
//    for(i = 0; i < DATA_SIZE; i++)
//    {
//         printf("%f ", results[i]);
//    }
//    printf("\n");

    // cleanup - release OpenCL resources
    clReleaseMemObject(input);
    clReleaseMemObject(output);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);

    return 0;
}
