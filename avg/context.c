#include <stdio.h>
#include <stdlib.h>

#include "err.h"
#include "free.h"
#include "context.h"

context_t* context_create(cl_device_type dev_type)
{
    printf("\ncontext creating...\n");

    context_t* context = malloc(sizeof(*context));

    if (context != NULL)
    {
        int ret = 0;
        cl_int err;

        context->dev_type = dev_type;
        context->total_comp_units = 0;

        err = clGetPlatformIDs(0, NULL, &context->plat_count);
        printf("clGetPlatformIDs \t [%s]\n", err_to_str(err));
        if (err == CL_SUCCESS)
        {
            printf("platforms count: %d\n", context->plat_count);

            context->plat = malloc(sizeof(*context->plat) * context->plat_count);

            err = clGetPlatformIDs(context->plat_count, context->plat, NULL);
            printf("clGetPlatformIDs \t [%s]\n", err_to_str(err));
            if (err == CL_SUCCESS)
            {
                context->dev_on_plat = malloc(sizeof(*context->dev_on_plat) * context->plat_count);

                context->dev = malloc(sizeof(*context->dev) * context->plat_count);
                context->dev_prop = malloc(sizeof(*context->dev_prop) * context->plat_count);
                context->contexts = malloc(sizeof(*context->contexts) * context->plat_count);
                context->cmd = malloc(sizeof(*context->cmd) * context->plat_count);

                int p;
                for (p = 0; p < context->plat_count; p++)
                {
                    err = clGetDeviceIDs(context->plat[p],
                                         context->dev_type,
                                         0,
                                         NULL,
                                         &context->dev_on_plat[p]);
                    printf("clGetDeviceIDs \t\t [%s]\n", err_to_str(err));
                    if (err == CL_SUCCESS)
                    {
#ifdef DEBUG
                        printf("platform: %d\t devices: %d\n", p, context->dev_on_plat[p]);
#endif
                        context->total_dev_count += context->dev_on_plat[p];

                        context->dev[p] = malloc(sizeof(**context->dev) * context->dev_on_plat[p]);
                        context->dev_prop[p] = malloc(sizeof(**context->dev_prop) * context->dev_on_plat[p]);
                        context->contexts[p] = malloc(sizeof(**context->contexts) * context->dev_on_plat[p]);
                        context->cmd[p] = malloc(sizeof(**context->cmd) * context->dev_on_plat[p]);

                        err = clGetDeviceIDs(context->plat[p],
                                             context->dev_type,
                                             context->dev_on_plat[p],
                                             context->dev[p],
                                             NULL);
                        printf("clGetDeviceIDs \t\t [%s]\n", err_to_str(err));
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
                                context->dev_prop[p][d].max_comp_units = 0;
                                err = clGetDeviceInfo(context->dev[p][d],
                                                      CL_DEVICE_MAX_COMPUTE_UNITS,
                                                      sizeof(context->dev_prop[p][d].max_comp_units),
                                                      &context->dev_prop[p][d].max_comp_units,
                                                      NULL);
                                printf("clGetDeviceInfo \t [%s]\n", err_to_str(err));
                                printf("CL_DEVICE_MAX_COMPUTE_UNITS: %d\n", context->dev_prop[p][d].max_comp_units);

                                context->total_comp_units += context->dev_prop[p][d].max_comp_units;
#ifdef DEBUG
                                printf("\nplat: %d\t dev: %d\n", p, d);
#endif
                                context->contexts[p][d] = clCreateContext(properties,
                                                                      1,
                                                                      &context->dev[p][d],
                                                                      NULL,
                                                                      NULL,
                                                                      &err);
                                printf("clCreateContext \t [%s]\n", err_to_str(err));
                                if (err == CL_SUCCESS)
                                {
                                    context->cmd[p][d] = clCreateCommandQueue(context->contexts[p][d],
                                                                              context->dev[p][d],
                                                                              CL_QUEUE_PROFILING_ENABLE,
                                                                              &err);
                                    printf("clCreateCommandQueue \t [%s]\n", err_to_str(err));
                                    if (err != CL_SUCCESS)
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
                ret = 2;
            }
        }
        else
        {
            ret = 1;
        }

        if (ret != 0)
        {
            context_clear(context);
            context = NULL;
        }
    }

    return context;
}

void context_clear(context_t* context)
{
    if (context == NULL)
        return;

    printf("\ncontext clearing...\n");

    int p, d;
    cl_int err;
    for (p = 0; p < context->plat_count; p++)
    {
        for (d = 0; d < context->dev_on_plat[p]; d++)
        {
            err = clReleaseCommandQueue(context->cmd[p][d]);
            printf("clReleaseCommandQueue \t [%s]\n", err_to_str(err));
            err = clReleaseContext(context->contexts[p][d]);
            printf("clReleaseContext \t [%s]\n", err_to_str(err));
        }
    }

    printf("stage 1\n");
    free_ptr_2d((void**) context->cmd, context->plat_count);
    printf("stage 2\n");
    free_ptr_2d((void**) context->contexts, context->plat_count);
    printf("stage 3\n");
    free_ptr_2d((void**) context->dev_prop, context->plat_count);
    printf("stage 4\n");
    free_ptr_2d((void**) context->dev, context->plat_count);

    printf("stage 5\n");
    free_ptr_1d(context->dev_on_plat);
    printf("stage 6\n");
    free_ptr_1d(context->plat);

    context->plat_count = 0;
    context->total_dev_count = 0;
    context->dev_type = CL_DEVICE_TYPE_GPU;

    printf("stage 7\n");
    free_ptr_1d(context);
    printf("stage end\n");
}
