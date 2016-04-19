#include "free.h"
#include "context.h"

context_t* context_create(cl_device_type dev_type)
{
    int ret = 0;

    context_t* context = malloc(sizeof(*context));

#ifdef DEBUG
    printf("init\n");
#endif

#ifdef DEBUG
    char err_str[MAX_STR_ERR_LEN];
#endif
    context->dev_type = dev_type;

    if (clGetPlatformIDs(0, NULL, &context->plat_count) == CL_SUCCESS)
    {
#ifdef DEBUG
        printf("platforms count: %d\n", context->plat_count);
#endif
        context->plat = malloc(sizeof(*context->plat) * context->plat_count);

        if (clGetPlatformIDs(context->plat_count,
                             context->plat,
                             NULL) == CL_SUCCESS)
        {
            context->dev_on_plat = malloc(sizeof(*context->dev_on_plat) * context->plat_count);

            context->dev = malloc(sizeof(*context->dev) * context->plat_count);
//            context->dev_prop = malloc(sizeof(*context->dev_prop) * context->plat_count);
            context->contexts = malloc(sizeof(*contexts) * context->plat_count);
            context->cmd = malloc(sizeof(*context->cmd) * context->plat_count);

            int p;
            for (p = 0; p < context->plat_count; p++)
            {
                if (clGetDeviceIDs(context->plat[p],
                                   context->dev_type,
                                   0,
                                   NULL,
                                   &context->dev_on_plat[p]) == CL_SUCCESS)
                {
#ifdef DEBUG
                    printf("platform: %d\t devices: %d\n", p, context->dev_on_plat[p]);
#endif
                    context->total_dev_count += context->dev_on_plat[p];

                    context->dev[p] = malloc(sizeof(**context->dev) * context->dev_on_plat[p];
//                   context->dev_prop[p] = malloc(sizeof(**context->dev_prop) * context->dev_on_plat[p]);
                    context->contexts[p] = malloc(sizeof(**context->contexts) * context->dev_on_plat[p]);
                    context->cmd[p] = malloc(sizeof(**context->cmd) * context->dev_on_plat[p]);




                    cl_int *err = NULL;

                    if (clGetDeviceIDs(context->plat[p],
                                       context->dev_type,
                                       context->dev_on_plat[p],
                                       context->dev[p],
                                       NULL) == CL_SUCCESS)
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
                                                                  err);
                            if (err == NULL)
                            {
                                context->cmd[p][d] = clCreateCommandQueue(context->contexts[p][d],
                                                                          context->dev[p][d],
                                                                          CL_QUEUE_PROFILING_ENABLE,
                                                                          err);
                                if (err != NULL)
                                {
#ifdef DEBUG
                                    err_to_str((*err), err_str);
                                    printf("clCreateCommandQueue [ %s ]\n", err_str);
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

void context_clear(context_t* context)
{
    int p, d;
    for (p = 0; p < context->plat_count; p++)
    {
        for (d = 0; d < context->dev_on_plat[p]; d++)
        {
            clReleaseCommandQueue(context->cmd[p][d]);
            clReleaseContext(context->contexts[p][d]);
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
