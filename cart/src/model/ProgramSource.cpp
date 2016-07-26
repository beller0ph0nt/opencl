#include <iostream>
#include <stdexcept>

#include "ProgramSource.h"

void ProgramSource::ProgramSource(Context *context, const string source_path)
{
    _source_path = source_path;
    _source = ReadFile(_source_path);

    program = new cl_program*[context->plat_count];

    for (cl_uint p = 0; p < context->plat_count; p++)
    {
        program[p] = new cl_program[context->dev_count[p]];

        for (cl_uint d = 0; d < context->dev_count[p]; d++)
        {
            cl_int err;
            program[p][d] = clCreateProgramWithSource(context->context[p][d], 1, (const char **) _source.c_str(), NULL, &err);
            if (err == CL_SUCCESS)
            {
                err = clBuildProgram(program[p][d], 0, NULL, BUILD_OPTIONS, NULL, NULL);
                if (err != CL_SUCCESS)
                {
                    char buf[4096];
                    size_t len;
                    clGetProgramBuildInfo(program[p][d], context->dev[p][d], CL_PROGRAM_BUILD_LOG, sizeof(buf), buf, &len);

                    std::cout << buf << std::endl;
                    throw runtime_error("error: clBuildProgram");
                }
            }
            else
            {
                throw runtime_error("error: clCreateProgramWithSource");
            }
        }
    }
}
