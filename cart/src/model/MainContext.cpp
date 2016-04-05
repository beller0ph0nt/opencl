#include "MainContext.h"

MainContext::MainContext(cl_device_type_t dev_type)
{
    data = new MainContextData;

    if (clGetPlatformIDs(0, NULL, &data->plat_count) == CL_SUCCESS)
    {
        data->plat = new cl_platform_id_t[data->plat_count];

        if (clGetPlatformIDs(data->plat_count, data->plat, NULL) == CL_SUCCESS)
        {
            data->dev_count = new cl_uint_t[data->plat_count];
            data->dev = new cl_device_id_t*[data->plat_count];
            data->context = new cl_context_t*[data->plat_count];
            data->cmd = new cl_command_queue_t*[data->plat_count];

            for (cl_uint_t p = 0; p < data->plat_count; p++)
            {
                if (clGetDeviceIDs(data->plat[p],
                                   dev_type,
                                   0,
                                   NULL,
                                   &data->dev_count[p]) == CL_SUCCESS)
                {
                    data->total_dev_count += data->dev_count[p];
                    data->dev[p] = new cl_device_id_t[data->dev_count[p]];
                    data->context[p] = new cl_context_t[data->dev_count[p]];
                    data->cmd[p] = new cl_command_queue_t[data->dev_count[p]];


                    cl_int_t *err = NULL;

                    if (clGetDeviceIDs(data->plat[p],
                                       dev_type,
                                       data->dev_count[p],
                                       data->dev[p],
                                       NULL) == CL_SUCCESS)
                    {
                        cl_context_properties_t properties[3] = {
                            CL_CONTEXT_PLATFORM,
                            (cl_context_properties_t) data->plat[p],
                            0
                        };

                        for (cl_uint_t d = 0; d < data->dev_count[p]; d++)
                        {
                            data->context[p][d] = clCreateContext(properties,
                                                                  1,
                                                                  &data->dev[p][d],
                                                                  NULL,
                                                                  NULL,
                                                                  err);
                            if (err == NULL)
                            {
                                data->cmd[p][d] = clCreateCommandQueue(data->context[p][d],
                                                                       data->dev[p][d],
                                                                       CL_QUEUE_PROFILING_ENABLE,
                                                                       err);
                                if (err != NULL)
                                {
                                }
                            }
                            else
                            {

                            }
                        }
                    }
                }
                else
                {

                }
            }
        }
        else
        {

        }
    }
    else
    {
    }
}

MainContext::~MainContext()
{
    for (cl_uint_t p = 0; p < data->plat_count; p++)
    {
        for (cl_uint_t d = 0; d < data->dev_count[p]; d++)
        {
            clReleaseCommandQueue(data->cmd[p][d]);
            clReleaseContext(data->context[p][d]);
        }

        delete[] data->cmd[p];
        delete[] data->context[p];
        delete[] data->dev[p];
    }

    delete[] data->cmd;
    delete[] data->context;
    delete[] data->dev;

    delete[] data->dev_count;
    delete[] data->plat;

    delete data;
}
