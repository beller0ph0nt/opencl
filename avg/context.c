#include "free.h"
#include "context.h"

context_t* context_create(cl_device_type dev_type)
{
    context_t* context = malloc(sizeof(*context));

    if (context != NULL)
    {
        int ret = 0;
        cl_int err;

        context->dev_type = dev_type;

        err = clGetPlatformIDs(0, NULL, &context->plat_count);
        if (err == CL_SUCCESS)
        {
#ifdef DEBUG
            printf("platforms count: %d\n", context->plat_count);
#endif
            context->plat = malloc(sizeof(*context->plat) * context->plat_count);

            err = clGetPlatformIDs(context->plat_count, context->plat, NULL);
            if (err == CL_SUCCESS)
            {
                context->dev_on_plat = malloc(sizeof(*context->dev_on_plat) * context->plat_count);

                context->dev = malloc(sizeof(*context->dev) * context->plat_count);
    //            context->dev_prop = malloc(sizeof(*context->dev_prop) * context->plat_count);
                context->contexts = malloc(sizeof(*contexts) * context->plat_count);
                context->cmd = malloc(sizeof(*context->cmd) * context->plat_count);

                int p;
                for (p = 0; p < context->plat_count; p++)
                {
                    err = clGetDeviceIDs(context->plat[p],
                                         context->dev_type,
                                         0,
                                         NULL,
                                         &context->dev_on_plat[p]);
                    if (err == CL_SUCCESS)
                    {
#ifdef DEBUG
                        printf("platform: %d\t devices: %d\n", p, context->dev_on_plat[p]);
#endif
                        context->total_dev_count += context->dev_on_plat[p];

                        context->dev[p] = malloc(sizeof(**context->dev) * context->dev_on_plat[p]);
    //                   context->dev_prop[p] = malloc(sizeof(**context->dev_prop) * context->dev_on_plat[p]);
                        context->contexts[p] = malloc(sizeof(**context->contexts) * context->dev_on_plat[p]);
                        context->cmd[p] = malloc(sizeof(**context->cmd) * context->dev_on_plat[p]);

                        err = clGetDeviceIDs(context->plat[p],
                                             context->dev_type,
                                             context->dev_on_plat[p],
                                             context->dev[p],
                                             NULL);
                        if (err == CL_SUCCESS)
                        {
                            cl_context_properties properties[3] = {
                                CL_CONTEXT_PLATFORM,
                                (cl_context_properties) context->plat[p],
                                0
                            };

                            int d;
                            for (d = 0; d < context->dev_on_plat[p]; d++)
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
                                printf("plat: %d\t dev: %d\n", p, d);
#endif
                                context->contexts[p][d] = clCreateContext(properties,
                                                                      1,
                                                                      &context->dev[p][d],
                                                                      NULL,
                                                                      NULL,
                                                                      &err);
                                if (err == CL_SUCCESS)
                                {
                                    context->cmd[p][d] = clCreateCommandQueue(context->contexts[p][d],
                                                                              context->dev[p][d],
                                                                              CL_QUEUE_PROFILING_ENABLE,
                                                                              &err);
                                    if (err != CL_SUCCESS)
                                    {
#ifdef DEBUG
                                        printf("clCreateCommandQueue [ %s ]\n", err_to_str(err));
#endif
                                        ret = 6;
                                        break;
                                    }
                                }
                                else
                                {
#ifdef DEBUG
                                    printf("clCreateContext [ %s ]\n", err_to_str(err));
#endif
                                    ret = 5;
                                    break;
                                }
                            }
                        }
                        else
                        {
#ifdef DEBUG
                            printf("clGetDeviceIDs [ %s ]\n", err_to_str(err));
#endif
                            ret = 4;
                        }
                    }
                    else
                    {
#ifdef DEBUG
                        printf("clGetDeviceIDs [ %s ]\n", err_to_str(err));
#endif
                        ret = 3;
                    }
                }
            }
            else
            {
#ifdef DEBUG
                printf("clGetPlatformIDs [ %s ]\n", err_to_str(err));
#endif
                ret = 2;
            }
        }
        else
        {
#ifdef DEBUG
            printf("clGetPlatformIDs [ %s ]\n", err_to_str(err));
#endif
            ret = 1;
        }

        if (ret != 0)
        {
            context_clear(context);
        }
    }

    return context;
}

void context_clear(context_t* context)
{
    int p, d;
    cl_int err;
    for (p = 0; p < context->plat_count; p++)
    {
        for (d = 0; d < context->dev_on_plat[p]; d++)
        {
            err = clReleaseCommandQueue(context->cmd[p][d]);
#ifdef DEBUG
            printf("clReleaseCommandQueue [ %s ]\n", err_to_str(err));
#endif
            err = clReleaseContext(context->contexts[p][d]);
#ifdef DEBUG
            printf("clReleaseContext [ %s ]\n", err_to_str(err));
#endif
        }
    }

    free_ptr_2d((void**) context->cmd, context->plat_count);
    free_ptr_2d((void**) context->contexts, context->plat_count);
//    free_ptr_2d((void**) context->dev_prop, context->plat_count);
    free_ptr_2d((void**) context->dev, context->plat_count);

    free_ptr_1d(context->dev_on_plat);
    free_ptr_1d(context->plat);

    context->plat_count = 0;
    context->total_dev_count = 0;
    context->dev_type = CL_DEVICE_TYPE_GPU;

    free_ptr_1d(context);
}
