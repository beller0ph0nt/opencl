#include "Context.h"

Context::Context(const cl_device_type dev_type)
{
    if (clGetPlatformIDs(0, NULL, &plat_count) == CL_SUCCESS)
    {
        plat = new cl_platform_id[plat_count];

        if (clGetPlatformIDs(plat_count, plat, NULL) == CL_SUCCESS)
        {
            dev_count = new cl_uint[plat_count];
            dev = new cl_device_id*[plat_count];
            context = new cl_context*[plat_count];
            cmd = new cl_command_queue*[plat_count];

            for (cl_uint p = 0; p < plat_count; p++)
            {
                if (clGetDeviceIDs(plat[p],
                                   dev_type,
                                   0,
                                   NULL,
                                   &dev_count[p]) == CL_SUCCESS)
                {
                    total_dev_count += dev_count[p];
                    dev[p] = new cl_device_id[dev_count[p]];
                    context[p] = new cl_context[dev_count[p]];
                    cmd[p] = new cl_command_queue[dev_count[p]];

                    if (clGetDeviceIDs(plat[p],
                                       dev_type,
                                       dev_count[p],
                                       dev[p],
                                       NULL) == CL_SUCCESS)
                    {
                        cl_context_properties properties[3] = {
                            CL_CONTEXT_PLATFORM,
                            (cl_context_properties) plat[p],
                            0
                        };

                        for (cl_uint d = 0; d < dev_count[p]; d++)
                        {
                            cl_int err;
                            context[p][d] = clCreateContext(properties,
                                                            1,
                                                            &dev[p][d],
                                                            NULL,
                                                            NULL,
                                                            &err);
                            if (err == CL_SUCCESS)
                            {
                                cmd[p][d] = clCreateCommandQueue(context[p][d],
                                                                 dev[p][d],
                                                                 CL_QUEUE_PROFILING_ENABLE,
                                                                 &err);
                                if (err != CL_SUCCESS)
                                {
                                    throw 1;
                                }
                            }
                            else
                            {
                                throw 2;
                            }
                        }
                    }
                    else
                    {
                        throw 3;
                    }
                }
                else
                {
                    throw 4;
                }
            }
        }
        else
        {
            throw 5;
        }
    }
    else
    {
        throw 6;
    }
}

Context::~Context()
{
    for (cl_uint p = 0; p < plat_count; p++)
    {
        for (cl_uint d = 0; d < dev_count[p]; d++)
        {
            cl_int err;
            err = clReleaseCommandQueue(cmd[p][d]);
            if (err != CL_SUCCESS)
            {
                throw 1;
            }

            err = clReleaseContext(context[p][d]);
            if (err != CL_SUCCESS)
            {
                throw 2;
            }
        }

        delete[] cmd[p];
        delete[] context[p];
        delete[] dev[p];
    }

    delete[] cmd;
    delete[] context;
    delete[] dev;

    delete[] dev_count;
    delete[] plat;
}
