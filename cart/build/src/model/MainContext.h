#ifndef __MAIN_CONTEXT_H
#define __MAIN_CONTEXT_H

#include <map>
using namespace std;

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.h>

typedef int PlatIndex;
typedef int DevIndex;

struct MainContextData
{
    cl_uint total_dev_count = 0;
    map<PlatIndex, cl_uint> dev_on_plat;
    map<PlatIndex, map<DevIndex, cl_device_id>> devices;

    cl_uint platforms_count = 0;
    map<PlatIndex, cl_platform_id> platforms;

    map<PlatIndex, map<DevIndex, cl_context>> contexts;
    map<PlatIndex, map<DevIndex, cl_command_queue>> cmd_queues;
};

class MainContext
{
public:
    MainContext();
    ~MainContext();
private:
    struct MainContextData *data;
};

#endif // __MAIN_CONTEXT_H
