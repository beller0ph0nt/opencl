#ifndef __DEVICE_H
#define __DEVICE_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#include "Platform.h"

class Device : public Platform
{
public:
    Device();
    ~Device();

private:
    cl_uint totalDeviceCount_ = 0;
    cl_uint* deviceCount_ = NULL;
}

#endif // __DEVICE_H
