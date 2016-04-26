#include "program.h"

program_t* program_create_src(const context_t* context,
                              const char* src_path,
                              const char *prog_name)
{
    program_t* prog = malloc(sizeof(*prog));

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
                        printf("clCreateProgramWithSource [ %s ]\n", err_to_str(err));
                        if (err == CL_SUCCESS)
                        {
                            err = clBuildProgram(prog->programs[plat][dev],
                                                 0,
                                                 NULL,
                                                 BUILD_OPTIONS,
                                                 NULL,
                                                 NULL);
                            printf("clBuildProgram [ %s ]\n", err_to_str(err));
                            if (err != CL_SUCCESS)
                            {
                                ret = 4;
                                printf("Error building program\n");

                                char buffer[4096];
                                size_t length;
                                clGetProgramBuildInfo(prog->programs[plat][dev],
                                                      context->dev[plat][dev],
                                                      CL_PROGRAM_BUILD_LOG,
                                                      sizeof(buffer),
                                                      buffer,
                                                      &length);

                                printf("%s\n", buffer);
                            }
                        }
                        else
                        {
                            ret = 3;
                            break;
                        }
                    }
                }
            }
            else
            {
                printf("kernel_read [ ERROR ]\n");
                ret = 2;
            }

            free_ptr_2d((void**) kernel_src, 1);
        }
        else
        {
            printf("kernel_len [ ERROR ]\n");
            ret = 1;
        }

        if (ret != 0)
        {
            program_clear(context, prog);
        }
    }

    return prog;
}

program_t* program_create_bin(const context_t* context, const char* bin_path, const char *prog_name)
{
    return NULL;
}

void program_clear(context_t* context, program_t* prog)
{
    int i, j;
    cl_int err;
    for (i = 0; i < context->plat_count; i++)
    {
        for (j = 0; j < context->dev_on_plat[i]; j++)
        {
            clReleaseProgram(prog->programs[i][j]);
            printf("clReleaseProgram [ %s ]\n", err_to_str(err));
        }
    }

    free_ptr_2d((void**) prog->programs, context->plat_count);
    free_ptr_1d(prog->prog_name);
    free_ptr_1d(prog);
}
