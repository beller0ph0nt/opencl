#ifndef __PROGRAM_H
#define __PROGRAM_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#include <string>
#include <fstream>
#include <sstream>
using namespace std;

#include "Context.h"

#define BUILD_OPTIONS   "-I ./"

class Program
{
protected:
    cl_program **program = NULL;
    Context *context = NULL;

    Program(Context *context):context(context) {}
    ~Program() {}

    virtual void Create(const string file_path) = 0;
    void ReadFile(const string file_path, string *src);
};

#endif // __PROGRAM_H
