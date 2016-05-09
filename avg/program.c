#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "file.h"
#include "free.h"
#include "program.h"

program_t* program_create_src(const context_t* context,
                              const char* src_path,
                              const char* prog_name,
                              const char* build_options)
{
    printf("\nprogram src creating...\n");

    program_t* prog = malloc(sizeof(*prog));
    memset(prog, 0, sizeof(*prog));

    if (prog != NULL)
    {
        int ret = 0;

        cl_int err;
        int len;
        if (file_len(src_path, &len) == 0)
        {
            char** kernel_src = NULL;
            kernel_src = malloc(sizeof(*kernel_src));
            kernel_src[0] = malloc(sizeof(**kernel_src) * (len + 1));
            kernel_src[0][len] = 0;

            if (file_read(src_path, len, kernel_src[0]) == 0)
            {
                prog->programs = malloc(sizeof(*prog->programs) * context->plat_count);
                prog->prog_name = malloc(sizeof(*prog->prog_name) * strlen(prog_name));
                strcpy(prog->prog_name, prog_name);

                int plat;
                for (plat = 0; plat < context->plat_count; plat++)
                {
                    prog->programs[plat] = malloc(sizeof(**prog->programs) * context->dev_on_plat[plat]);

                    int dev;
                    for (dev = 0; dev < context->dev_on_plat[plat]; dev++)
                    {
                        prog->programs[plat][dev] = clCreateProgramWithSource(context->contexts[plat][dev],
                                                                              1,
                                                                              (const char **) kernel_src,
                                                                              NULL,
                                                                              &err);
                        printf("clCreateProgramWithSource \t [%s]\n", err_to_str(err));
                        if (err == CL_SUCCESS)
                        {
                            err = clBuildProgram(prog->programs[plat][dev],
                                                 0,
                                                 NULL,
                                                 build_options,
                                                 NULL,
                                                 NULL);
                            printf("clBuildProgram \t\t\t [%s]\n", err_to_str(err));
                            if (err != CL_SUCCESS)
                            {
                                ret = 4;
                                char buffer[4096];
                                size_t length;
                                err = clGetProgramBuildInfo(prog->programs[plat][dev],
                                                            context->dev[plat][dev],
                                                            CL_PROGRAM_BUILD_LOG,
                                                            sizeof(buffer),
                                                            buffer,
                                                            &length);
                                printf("clGetProgramBuildInfo \t\t [%s]\n", err_to_str(err));
                                printf("%s\n", buffer);
                            }
                        }
                        else
                        {
                            ret = 3;
                        }
                    }
                }
            }
            else
            {
                ret = 2;
            }

            free_ptr_2d((void**) kernel_src, 1);
        }
        else
        {
            ret = 1;
        }

        if (ret != 0)
        {
            program_clear(context, prog);
            prog = NULL;
        }
    }

    return prog;
}

/*

    program_src2bin(src_path)

    program = clCreateProgramWithSource(clctx, 1, &dumbkernelsource, NULL, &errcode);
    errcode = clBuildProgram(program, env->num_devices, env->device, NULL, NULL, NULL);
    int number_of_binaries;
    char **binary;
    int *binary_sizes;
    errcode = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, NULL, 0, &number_of_binaries);
    binary_sizes = new int[number_of_binaries];
    binary = new char*[number_of_binaries];
    errcode = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, binary_sizes, number_of_binaries*sizeof(int), &number_of_binaries);
    for (int i = 0; i < number_of_binaries; ++i) binary[i] = new char[binary_sizes[i]];
    errcode = clGetProgramInfo(program, CL_PROGRAM_BINARIES, binary, number_of_binaries*sizeof(char*), &number_of_binaries);
 */

program_t* program_create_bin(const context_t* context,
                              const char* bin_path,
                              const char *prog_name)
{
    printf("\nprogram bin creating...\n");
    return NULL;
}

void program_clear(const context_t* context, program_t* prog)
{
    if (context == NULL || prog == NULL)
        return;

    printf("\nprogram clearing...\n");

    int i, j;
    cl_int err;
    for (i = 0; i < context->plat_count; i++)
    {
        for (j = 0; j < context->dev_on_plat[i]; j++)
        {
            if (prog->programs != NULL)
            {
                err = clReleaseProgram(prog->programs[i][j]);
                printf("clReleaseProgram \t [%s]\n", err_to_str(err));
            }
        }
    }

    free_ptr_2d((void**) prog->programs, context->plat_count);
    free_ptr_1d(prog->prog_name);
    free_ptr_1d(prog);
}
