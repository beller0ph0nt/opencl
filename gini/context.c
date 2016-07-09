#include <stdio.h>
#include <stdlib.h>

#include "err.h"
#include "free.h"
#include "context.h"
#include "wrap_memory.h"
#include "wrap_opencl.h"

context_t* context_create(cl_device_type dev_type)
{
    context_t *context = _wrp_malloc(sizeof(*context));

    context->dev_type = dev_type;
    context->total_comp_units = 0;

    _wrp_clGetPlatformIDs(0, NULL, &context->plat_count);

    context->plat = _wrp_malloc(sizeof(*context->plat) * context->plat_count);

    _wrp_clGetPlatformIDs(context->plat_count, context->plat, NULL);

    context->dev_on_plat = malloc(sizeof(*context->dev_on_plat) * context->plat_count);

    context->dev = _wrp_malloc(sizeof(*context->dev) * context->plat_count);
    context->dev_prop = _wrp_malloc(sizeof(*context->dev_prop) * context->plat_count);
    context->contexts = _wrp_malloc(sizeof(*context->contexts) * context->plat_count);
    context->cmd = _wrp_malloc(sizeof(*context->cmd) * context->plat_count);

    int p, d;
    for (p = 0; p < context->plat_count; p++)
    {
        _wrp_clGetDeviceIDs(context->plat[p], context->dev_type, 0, NULL, &context->dev_on_plat[p]);

        context->total_dev_count += context->dev_on_plat[p];

        context->dev[p] = _wrp_malloc(sizeof(**context->dev) * context->dev_on_plat[p]);
        context->dev_prop[p] = _wrp_malloc(sizeof(**context->dev_prop) * context->dev_on_plat[p]);
        context->contexts[p] = _wrp_malloc(sizeof(**context->contexts) * context->dev_on_plat[p]);
        context->cmd[p] = _wrp_malloc(sizeof(**context->cmd) * context->dev_on_plat[p]);

        _wrp_clGetDeviceIDs(context->plat[p], context->dev_type, context->dev_on_plat[p], context->dev[p], NULL);

        cl_context_properties properties[3] = {
            CL_CONTEXT_PLATFORM,
            (cl_context_properties) context->plat[p],
            0
        };

        for (d = 0; d < context->dev_on_plat[p]; d++)
        {
            _wrp_clGetDeviceInfo(context->dev[p][d], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(context->dev_prop[p][d].max_comp_units), &context->dev_prop[p][d].max_comp_units, NULL);
            _wrp_clGetDeviceInfo(context->dev[p][d], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(context->dev_prop[p][d].glob_mem_size), &context->dev_prop[p][d].glob_mem_size, NULL);
            _wrp_clGetDeviceInfo(context->dev[p][d], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(context->dev_prop[p][d].max_mem_alloc_size), &context->dev_prop[p][d].max_mem_alloc_size, NULL);

            context->total_comp_units += context->dev_prop[p][d].max_comp_units;

            context->contexts[p][d] = _wrp_clCreateContext(properties, DEV_PER_CONTEXT, &context->dev[p][d], NULL, NULL);
            context->cmd[p][d] = _wrp_clCreateCommandQueue(context->contexts[p][d], context->dev[p][d], CL_QUEUE_PROFILING_ENABLE);
        }
    }

    return context;
}

void context_clear(context_t *context)
{
    if (context == NULL)
        return;

    int p, d;
    for (p = 0; p < context->plat_count; p++)
    {
        for (d = 0; d < context->dev_on_plat[p]; d++)
        {
            _wrp_clReleaseCommandQueue(context->cmd[p][d]);
            _wrp_clReleaseContext(context->contexts[p][d]);
        }
    }

    free_ptr_2d((void**) context->cmd, context->plat_count);
    free_ptr_2d((void**) context->contexts, context->plat_count);
    free_ptr_2d((void**) context->dev_prop, context->plat_count);
    free_ptr_2d((void**) context->dev, context->plat_count);

    free_ptr_1d(context->dev_on_plat);
    free_ptr_1d(context->plat);

    context->plat_count = 0;
    context->total_dev_count = 0;

    free_ptr_1d(context);
}
