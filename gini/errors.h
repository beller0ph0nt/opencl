#ifndef __ERR_H
#define __ERR_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#define FILE_OPEN_ERROR     1
#define FILE_SEEK_ERROR     2
#define FILE_TELL_ERROR     3
#define FILE_CLOSE_ERROR    4
#define FILE_READ_ERROR     5

#define MEMORY_MALLOC_ERROR 6

#define OPENCL_ERROR        7

const char* err_to_str(const cl_uint err);

#endif // __ERR_H
