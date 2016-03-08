#include <CL/cl.h>
#include <string.h>

void err_to_str(const cl_uint err, char *str)
{
    switch (err)
    {
    case CL_SUCCESS:
        strcpy(str, "CL_SUCCESS");
        break;
    case CL_DEVICE_NOT_FOUND:
        strcpy(str, "CL_DEVICE_NOT_FOUND");
        break;
    case CL_DEVICE_NOT_AVAILABLE:
        strcpy(str, "CL_DEVICE_NOT_AVAILABLE");
        break;
    case CL_COMPILER_NOT_AVAILABLE:
        strcpy(str, "CL_COMPILER_NOT_AVAILABLE");
        break;
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        strcpy(str, "CL_MEM_OBJECT_ALLOCATION_FAILURE");
        break;
    case CL_OUT_OF_RESOURCES:
        strcpy(str, "CL_OUT_OF_RESOURCES");
        break;
    case CL_OUT_OF_HOST_MEMORY:
        strcpy(str, "CL_OUT_OF_HOST_MEMORY");
        break;
    case CL_PROFILING_INFO_NOT_AVAILABLE:
        strcpy(str, "CL_PROFILING_INFO_NOT_AVAILABLE");
        break;
    case CL_MEM_COPY_OVERLAP:
        strcpy(str, "CL_MEM_COPY_OVERLAP");
        break;
    case CL_IMAGE_FORMAT_MISMATCH:
        strcpy(str, "CL_IMAGE_FORMAT_MISMATCH");
        break;
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:
        strcpy(str, "CL_IMAGE_FORMAT_NOT_SUPPORTED");
        break;
    case CL_BUILD_PROGRAM_FAILURE:
        strcpy(str, "CL_BUILD_PROGRAM_FAILURE");
        break;
    case CL_MAP_FAILURE:
        strcpy(str, "CL_MAP_FAILURE");
        break;
    case CL_MISALIGNED_SUB_BUFFER_OFFSET:
        strcpy(str, "CL_MISALIGNED_SUB_BUFFER_OFFSET");
        break;
    case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
        strcpy(str, "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST");
        break;
    case CL_COMPILE_PROGRAM_FAILURE:
        strcpy(str, "CL_COMPILE_PROGRAM_FAILURE");
        break;
    case CL_LINKER_NOT_AVAILABLE:
        strcpy(str, "CL_LINKER_NOT_AVAILABLE");
        break;
    case CL_LINK_PROGRAM_FAILURE:
        strcpy(str, "CL_LINK_PROGRAM_FAILURE");
        break;
    case CL_DEVICE_PARTITION_FAILED:
        strcpy(str, "CL_DEVICE_PARTITION_FAILED");
        break;
    case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:
        strcpy(str, "CL_KERNEL_ARG_INFO_NOT_AVAILABLE");
        break;
    case CL_INVALID_VALUE:
        strcpy(str, "CL_INVALID_VALUE");
        break;
    case CL_INVALID_DEVICE_TYPE:
        strcpy(str, "CL_INVALID_DEVICE_TYPE");
        break;
    case CL_INVALID_PLATFORM:
        strcpy(str, "CL_INVALID_PLATFORM");
        break;
    case CL_INVALID_DEVICE:
        strcpy(str, "CL_INVALID_DEVICE");
        break;
    case CL_INVALID_CONTEXT:
        strcpy(str, "CL_INVALID_CONTEXT");
        break;
    case CL_INVALID_QUEUE_PROPERTIES:
        strcpy(str, "CL_INVALID_QUEUE_PROPERTIES");
        break;
    case CL_INVALID_COMMAND_QUEUE:
        strcpy(str, "CL_INVALID_COMMAND_QUEUE");
        break;
    case CL_INVALID_HOST_PTR:
        strcpy(str, "CL_INVALID_HOST_PTR");
        break;
    case CL_INVALID_MEM_OBJECT:
        strcpy(str, "CL_INVALID_MEM_OBJECT");
        break;
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
        strcpy(str, "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR");
        break;
    case CL_INVALID_IMAGE_SIZE:
        strcpy(str, "CL_INVALID_IMAGE_SIZE");
        break;
    case CL_INVALID_SAMPLER:
        strcpy(str, "CL_INVALID_SAMPLER");
        break;
    case CL_INVALID_BINARY:
        strcpy(str, "CL_INVALID_BINARY");
        break;
    case CL_INVALID_BUILD_OPTIONS:
        strcpy(str, "CL_INVALID_BUILD_OPTIONS");
        break;
    case CL_INVALID_PROGRAM:
        strcpy(str, "CL_INVALID_PROGRAM");
        break;
    case CL_INVALID_PROGRAM_EXECUTABLE:
        strcpy(str, "CL_INVALID_PROGRAM_EXECUTABLE");
        break;
    case CL_INVALID_KERNEL_NAME:
        strcpy(str, "CL_INVALID_KERNEL_NAME");
        break;
    case CL_INVALID_KERNEL_DEFINITION:
        strcpy(str, "CL_INVALID_KERNEL_DEFINITION");
        break;
    case CL_INVALID_KERNEL:
        strcpy(str, "CL_INVALID_KERNEL");
        break;
    case CL_INVALID_ARG_INDEX:
        strcpy(str, "CL_INVALID_ARG_INDEX");
        break;
    case CL_INVALID_ARG_VALUE:
        strcpy(str, "CL_INVALID_ARG_VALUE");
        break;
    case CL_INVALID_ARG_SIZE:
        strcpy(str, "CL_INVALID_ARG_SIZE");
        break;
    case CL_INVALID_KERNEL_ARGS:
        strcpy(str, "CL_INVALID_KERNEL_ARGS");
        break;
    case CL_INVALID_WORK_DIMENSION:
        strcpy(str, "CL_INVALID_WORK_DIMENSION");
        break;
    case CL_INVALID_WORK_GROUP_SIZE:
        strcpy(str, "CL_INVALID_WORK_GROUP_SIZE");
        break;
    case CL_INVALID_WORK_ITEM_SIZE:
        strcpy(str, "CL_INVALID_WORK_ITEM_SIZE");
        break;
    case CL_INVALID_GLOBAL_OFFSET:
        strcpy(str, "CL_INVALID_GLOBAL_OFFSET");
        break;
    case CL_INVALID_EVENT_WAIT_LIST:
        strcpy(str, "CL_INVALID_EVENT_WAIT_LIST");
        break;
    case CL_INVALID_EVENT:
        strcpy(str, "CL_INVALID_EVENT");
        break;
    case CL_INVALID_OPERATION:
        strcpy(str, "CL_INVALID_OPERATION");
        break;
    case CL_INVALID_GL_OBJECT:
        strcpy(str, "CL_INVALID_GL_OBJECT");
        break;
    case CL_INVALID_BUFFER_SIZE:
        strcpy(str, "CL_INVALID_BUFFER_SIZE");
        break;
    case CL_INVALID_MIP_LEVEL:
        strcpy(str, "CL_INVALID_MIP_LEVEL");
        break;
    case CL_INVALID_GLOBAL_WORK_SIZE:
        strcpy(str, "CL_INVALID_GLOBAL_WORK_SIZE");
        break;
    case CL_INVALID_PROPERTY:
        strcpy(str, "CL_INVALID_PROPERTY");
        break;
    case CL_INVALID_IMAGE_DESCRIPTOR:
        strcpy(str, "CL_INVALID_IMAGE_DESCRIPTOR");
        break;
    case CL_INVALID_COMPILER_OPTIONS:
        strcpy(str, "CL_INVALID_COMPILER_OPTIONS");
        break;
    case CL_INVALID_LINKER_OPTIONS:
        strcpy(str, "CL_INVALID_LINKER_OPTIONS");
        break;
    case CL_INVALID_DEVICE_PARTITION_COUNT:
        strcpy(str, "CL_INVALID_DEVICE_PARTITION_COUNT");
        break;
    case CL_INVALID_PIPE_SIZE:
        strcpy(str, "CL_INVALID_PIPE_SIZE");
        break;
    case CL_INVALID_DEVICE_QUEUE:
        strcpy(str, "CL_INVALID_DEVICE_QUEUE");
        break;
    default:
        strcpy(str, "unknown");
        break;
    }
}
