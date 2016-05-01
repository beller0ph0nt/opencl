#ifndef __ERR_H
#define __ERR_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

const char* err_to_str(const cl_uint err);

#endif // __ERR_H
