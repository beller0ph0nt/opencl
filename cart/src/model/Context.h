#ifndef __CONTEXT_H
#define __CONTEXT_H

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

#include "Platform.h"

class Context : public Platform
{
public:
    Context(const cl_device_type deviceType = CL_DEVICE_TYPE_GPU);
    ~Context();

    inline cl_uint getDeviceCount() const;
    inline cl_uint getDeviceCount(cl_uint platformIndex) const;

    inline cl_device_id getDevice(cl_uint platformIndex, cl_uint deviceIndex) const;

    inline cl_context get(cl_uint platformIndex, cl_uint deviceIndex) const;

    inline cl_command_queue getCommandQueue(cl_uint platformIndex, cl_uint deviceIndex) const;

private:
    cl_uint totalDeviceCount_ = 0;
    cl_uint* deviceCount_ = NULL;

    cl_device_id** device_ = NULL;

    cl_context** context_ = NULL;

    cl_command_queue** commandQueue_ = NULL;
};

#endif // __CONTEXT_H
