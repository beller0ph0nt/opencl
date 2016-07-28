#ifndef __DEVICE_H
#define __DEVICE_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#include "Platform.h"

class Device : public Platform
{
private:
    cl_uint totalDeviceCount_ = 0;
    cl_uint* deviceCount_ = NULL;
    cl_device_id** device_ = NULL;

public:
    Device();
    ~Device();

    inline cl_uint getTotalCount() const;
    inline cl_uint getCount(cl_uint platformIndex) const;
    inline cl_device_id get(cl_uint platformIndex, cl_uint deviceIndex) const;
}

#endif // __DEVICE_H
