#include "Platform.h"
#include "OpenCLWrappers.h"

Platform::Platform()
{
    _wrp_clGetPlatformIDs(0, NULL, &platformCount_);
    platform_ = new cl_platform_id[platformCount_];
    _wrp_clGetPlatformIDs(platformCount_, platform_, NULL);
}

Platform::~Platform()
{
    delete[] platform_;
    platformCount_ = 0;
}

cl_platform_id Platform::getCount() const
{
    return platformCount_;
}

cl_platform_id Platform::get(cl_uint platformIndex) const
{
    return platform_[platformIndex];
}
