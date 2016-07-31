#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "def.h"
#include "file.h"
#include "free.h"
#include "errors.h"
#include "program.h"
#include "wrap_memory.h"
#include "wrap_opencl.h"

program_t*
program_create_src(const context_t* context,
                   const char*      src_path,
                   const char*      prog_name,
                   const char*      build_options)
{
    TRACE_DEBUG("begin");
    program_t* prog = _wrp_malloc(sizeof(program_t));

    long len = file_len(src_path);

    char** kernel_src = _wrp_malloc(sizeof(char*));
    kernel_src[0] = _wrp_malloc(sizeof(char) * (len + 1));

    file_read(src_path, len, kernel_src[0]);

    prog->programs =
        _wrp_malloc(sizeof(*prog->programs) * context->plat_count);
    prog->prog_name =
        _wrp_malloc(sizeof(*prog->prog_name) * strlen(prog_name));
    strcpy(prog->prog_name, prog_name);

    int plat, dev;
    for (plat = 0; plat < context->plat_count; plat++)
    {
        prog->programs[plat] =
            _wrp_malloc(sizeof(**prog->programs) * context->dev_on_plat[plat]);

        for (dev = 0; dev < context->dev_on_plat[plat]; dev++)
        {
            prog->programs[plat][dev] =
                _w_clCreateProgramWithSource(context->contexts[plat][dev],
                                             1,
                                             (const char **) kernel_src,
                                             NULL);
            _w_clBuildProgram(prog->programs[plat][dev],
                              context->dev[plat][dev],
                              0,
                              NULL,
                              build_options,
                              NULL,
                              NULL);
        }
    }

    free_ptr_2d((void**) kernel_src, 1);

    return prog;
}

void
program_clear(const context_t *context, program_t *prog)
{
    TRACE_DEBUG("begin");
    if (context == NULL || prog == NULL)
        return;

    int plat, dev;
    for (plat = 0; plat < context->plat_count; plat++)
    {
        for (dev = 0; dev < context->dev_on_plat[plat]; dev++)
        {
            if (prog->programs != NULL)
            {
                _w_clReleaseProgram(prog->programs[plat][dev]);
            }
        }
    }

    free_ptr_2d((void**) prog->programs, context->plat_count);
    free_ptr_1d(prog->prog_name);
    free_ptr_1d(prog);
}
