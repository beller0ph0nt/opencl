#include "MainContext.h"

MainContext::MainContext(cl_device_type dev_type)
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
    delete[] data->cmd;
    delete[] data->context;
    delete[] data->dev;
    delete[] data->dev_count;
    delete[] data->plat;
    delete data;
}
