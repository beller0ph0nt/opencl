#ifndef __PROGRAM_H
#define __PROGRAM_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#include <string>
using namespace std;

#include "Context.h"

#define BUILD_OPTIONS   "-I ./"

class Program
{
protected:
    cl_program **_program = NULL;
    Context *_context = NULL;

protected:
    string ReadFile(const string file_path);
};

#endif // __PROGRAM_H
