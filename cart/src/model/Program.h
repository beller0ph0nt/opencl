#ifndef __PROGRAM_H
#define __PROGRAM_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#include <string>
#include <fstream>
#include <sstream>
using namespace std;

#include "MainContext.h"

typedef cl_program cl_program_t;



class Program
{
protected:
    cl_program_t **program = NULL;
    MainContext &context;
public:
    Program(MainContext context):context(context) {}
    ~Program() {}
protected:
    virtual void Create(const string file_path) = 0;
    void ReadFile(const string file_path, string &src);
};



class ProgramSource : public Program
{
public:
    ProgramSource(MainContext context, const string file_path):Program(context) {}
protected:
    virtual void Create(const string file_path);
};



class ProgramBinary : public Program
{
public:
    ProgramBinary(MainContext context, const string file_path):Program(context) {}
protected:
    virtual void Create(const string file_path);
};

#endif // __PROGRAM_H
