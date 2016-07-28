#include "Context.h"
#include "OpenCLWrappers.h"

Context::Context(const cl_device_type deviceType)
{


    context_ = new cl_context*[Platform::getCount()];
    commandQueue_ = new cl_command_queue*[Platform::getCount()];

    for (cl_uint p = 0; p < Platform::getCount(); p++)
    {
        _wrp_clGetDeviceIDs(Platform::get(p), deviceType, 0, NULL, &deviceCount_[p]);

        totalDeviceCount_ += deviceCount_[p];

        device_[p] = new cl_device_id[deviceCount_[p]];
        context_[p] = new cl_context[deviceCount_[p]];
        commandQueue_[p] = new cl_command_queue[deviceCount_[p]];

        for (cl_uint d = 0; d < deviceCount_[p]; d++)
        {
            cl_context_properties properties[3] =
            {
                CL_CONTEXT_PLATFORM,
                (cl_context_properties) Platform::get(p),
                0
            };

            context_[p][d] = _wrp_clCreateContext(properties, 1, &device_[p][d], NULL, NULL);
            commandQueue_[p][d] = _wrp_clCreateCommandQueue(context_[p][d], device_[p][d], CL_QUEUE_PROFILING_ENABLE);
        }
    }
}

Context::~Context()
{
    for (cl_uint p = 0; p < Platform::getCount(); p++)
    {
        for (cl_uint d = 0; d < deviceCount_[p]; d++)
        {
            _wrp_clReleaseCommandQueue(commandQueue_[p][d]);
            _wrp_clReleaseContext(context_[p][d]);
        }

        delete[] commandQueue_[p];
        delete[] context_[p];
        delete[] device_[p];
    }

    delete[] commandQueue_;
    delete[] context_;
    delete[] device_;

    delete[] deviceCount_;
//    delete[] platform_;
}

cl_uint Context::getDeviceCount() const
{
    return totalDeviceCount_;
}

cl_uint Context::getDeviceCount(cl_uint platformIndex) const
{
    return deviceCount_[platformIndex];
}

cl_device_id Context::getDevice(cl_uint platformIndex, cl_uint deviceIndex) const
{
    return device_[platformIndex][deviceIndex];
}

cl_context Context::get(cl_uint platformIndex, cl_uint deviceIndex) const
{
    return context_[platformIndex][deviceIndex];
}

cl_command_queue Context::getCommandQueue(cl_uint platformIndex, cl_uint deviceIndex) const
{
}
