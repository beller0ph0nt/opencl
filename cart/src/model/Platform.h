#ifndef __PLATFORM_H
#define __PLATFORM_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

class Platform
{
private:
    cl_uint platformCount_ = 0;
    cl_platform_id* platform_ = NULL;

public:
    Platform();
    ~Platform();

    inline cl_uint getCount() const;
    inline cl_platform_id get(cl_uint platformIndex) const;
}

#endif // __PLATFORM_H
