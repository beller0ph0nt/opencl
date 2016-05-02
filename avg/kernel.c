#include <stdio.h>
#include <stdlib.h>

#include "err.h"
#include "free.h"
#include "kernel.h"

kernel_t* kernel_create(const context_t* context, const program_t* prog)
{
    printf("\nkernel creating...\n");

    kernel_t *kern = malloc(sizeof(*kern));

    if (kern != NULL)
    {
        int ret = 0;

        kern->kernels = malloc(sizeof(*kern->kernels) * context->plat_count);
        kern->prop = malloc(sizeof(*kern->prop) * context->plat_count);

        int plat;
        cl_int err;
        for (plat = 0; plat < context->plat_count; plat++)
        {
            kern->kernels[plat] = malloc(sizeof(**kern->kernels) * context->dev_on_plat[plat]);
            kern->prop[plat] = malloc(sizeof(**kern->prop) * context->dev_on_plat[plat]);

            int dev;
            for (dev = 0; dev < context->dev_on_plat[plat]; dev++)
            {
                kern->kernels[plat][dev] = clCreateKernel(prog->programs[plat][dev],
                                                          prog->prog_name,
                                                          &err);
                printf("clCreateKernel \t\t\t [%s]\n", err_to_str(err));
                if (err == CL_SUCCESS)
                {
                     err = clGetKernelWorkGroupInfo(kern->kernels[plat][dev],
                                                    context->dev[plat][dev],
                                                    CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,
                                                    sizeof(kern->prop[plat][dev].pref_work_group_size_mult),
                                                    &kern->prop[plat][dev].pref_work_group_size_mult,
                                                    NULL);
                     printf("clGetKernelWorkGroupInfo \t [%s]\n", err_to_str(err));
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
        }

        if (ret != 0)
        {
            kernel_clear(context, kern);
            kern = NULL;
        }
    }

    return kern;
}

void kernel_clear(const context_t *context, kernel_t* kernel)
{
    if (context == NULL || kernel == NULL)
        return;

    printf("\nkernel clearing...\n");

    int i, j;
    cl_int err;
    for (i = 0; i < context->plat_count; i++)
    {
        for (j = 0; j < context->dev_on_plat[i]; j++)
        {
            err = clReleaseKernel(kernel->kernels[i][j]);
            printf("clReleaseKernel \t\t [%s]\n", err_to_str(err));
        }
    }

    free_ptr_2d((void**) kernel->prop, context->plat_count);
    free_ptr_2d((void**) kernel->kernels, context->plat_count);
    free_ptr_1d(kernel);
}
