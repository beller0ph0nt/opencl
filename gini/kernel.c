#include <stdio.h>
#include <stdlib.h>

#include "free.h"
#include "error.h"
#include "kernel.h"
#include "wrap_memory.h"
#include "wrap_opencl.h"

kernel_t* kernel_create(const context_t* context, const program_t* prog)
{
    kernel_t *kern = _wrp_malloc(sizeof(*kern));

    kern->kernels = _wrp_malloc(sizeof(*kern->kernels) * context->plat_count);
    kern->prop = _wrp_malloc(sizeof(*kern->prop) * context->plat_count);

    int plat, dev;
    for (plat = 0; plat < context->plat_count; plat++)
    {
        kern->kernels[plat] =
            _wrp_malloc(sizeof(**kern->kernels) * context->dev_on_plat[plat]);

        kern->prop[plat] =
            _wrp_malloc(sizeof(**kern->prop) * context->dev_on_plat[plat]);

        for (dev = 0; dev < context->dev_on_plat[plat]; dev++)
        {
            kern->kernels[plat][dev] =
                _w_clCreateKernel(prog->programs[plat][dev],
                                  prog->prog_name);

            _w_clGetKernelWorkGroupInfo(kern->kernels[plat][dev],
                context->dev[plat][dev],
                CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,
                sizeof(kern->prop[plat][dev].pref_work_group_size_mult),
                &kern->prop[plat][dev].pref_work_group_size_mult,
                NULL);
        }
    }

    return kern;
}

void kernel_clear(const context_t *context, kernel_t* kernel)
{
    if (context == NULL || kernel == NULL)
        return;

    int i, j;
    for (i = 0; i < context->plat_count; i++)
    {
        for (j = 0; j < context->dev_on_plat[i]; j++)
        {
            _w_clReleaseKernel(kernel->kernels[i][j]);
        }
    }

    free_ptr_2d((void**) kernel->prop, context->plat_count);
    free_ptr_2d((void**) kernel->kernels, context->plat_count);
    free_ptr_1d(kernel);
}
