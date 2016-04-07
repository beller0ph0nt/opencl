#ifndef __KERNEL_CONTEXT_H
#define __KERNEL_CONTEXT_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#include <string>
#include <fstream>
#include <sstream>
using namespace std;

typedef cl_program cl_program_t;
typedef cl_kernel cl_kernel_t;

typedef struct {
    size_t  pref_work_group_size_mult;
} KernelProp;

class KernelContext
{
private:
    cl_program_t **program = NULL;
    cl_kernel_t **kernel = NULL;
    KernelProp **kernel_prop = NULL;
public:
    KernelContext(const string file_path);
    ~KernelContext();
    void ReadKernelSrc(const string file_path, string &src);
};

#endif // __KERNEL_CONTEXT_H
