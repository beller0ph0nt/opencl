#include "Device.h"

Device::Device()
{
    deviceCount_ = new cl_uint[Platform::getCount()];
    device_ = new cl_device_id*[Platform::getCount()];

    for (cl_uint p = 0; p < Platform::getCount(); p++)
    {
        _wrp_clGetDeviceIDs(Platform::get(p), deviceType, 0, NULL, &deviceCount_[p]);

        for (cl_uint d = 0; d < deviceCount_[p]; d++)
        {
        }
    }
}

Device::~Device()
{
}
