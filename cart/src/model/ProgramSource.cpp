#include <iostream>

#include "ProgramSource.h"

void ProgramSource::Create(const string file_path)
{
    string src;
    ReadFile(file_path, &src);
    program = new cl_program_t*[context->plat_count];

    for (cl_uint_t p = 0; p < context->plat_count; p++)
    {
        program[p] = new cl_program_t[context->dev_count[p]];

        for (cl_uint_t d = 0; d < context->dev_count[p]; d++)
        {
            cl_int_t err;
            program[p][d] = clCreateProgramWithSource(context->context[p][d],
                                                      1,
                                                      (const char **) src.c_str(),  // !!! возможно надо будет заводить временную переменную для корректного преобразования !!!
                                                      NULL,
                                                      &err);
            if (err == CL_SUCCESS)
            {
                err = clBuildProgram(program[p][d],
                                     0,
                                     NULL,
                                     BUILD_OPTIONS,
                                     NULL,
                                     NULL);
                if (err != CL_SUCCESS)
                {
                    char buf[4096];
                    size_t len;
                    clGetProgramBuildInfo(program[p][d],
                                          context->dev[p][d],
                                          CL_PROGRAM_BUILD_LOG,
                                          sizeof(buf),
                                          buf,
                                          &len);

                    std::cout << buf << std::endl;
                    throw 1;
                }
            }
            else
            {
                throw 2;
            }
        }
    }
}
