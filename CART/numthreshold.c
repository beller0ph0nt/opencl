#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <stdio.h>
#include <CL/cl.h>

#define DATA_SIZE 10

int main()
{
    unsigned int in_count = 1;
    float in_data[DATA_SIZE] = { 0 };
    float out_data[DATA_SIZE - 1] = { 0 };

    const char * kernel_src =
    "kernel void num_threshold(constant float * in,     \r\n"
    "                          constant uint * count,   \r\n"
    "                          global float * out)      \r\n"
    "{                                                  \r\n"
    "   size_t id = get_global_id(0);                   \r\n"
    "   uint cnt = *count;                              \r\n"
    "   uint i = id / cnt;                              \r\n"
    "   uint r = id % cnt;                              \r\n"
    "   uint e = cnt * i + r;                           \r\n"
    "                                                   \r\n"
    "   out[e] = (in[e] + in[e + 1]) / 2;               \r\n"
    "}                                                  \r\n";

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

    cl_program program = clCreateProgramWithSource(context, 1, (const char **) &kernel_src, NULL, &err);

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
    //cl_mem size   = clCreateBuffer(context, CL_MEM_READ_ONLY,  sizeof(unsigned int),            NULL, NULL);
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
