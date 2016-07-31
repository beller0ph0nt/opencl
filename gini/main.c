#include "def.h"
#include "context.h"
#include "program.h"

int main()
{
    TRACE_DEBUG("begin");
    context_t* context = context_create(CL_DEVICE_TYPE_GPU);
    program_t* program = program_create_src(context, "gini.cl", "gini", "");

    program_clear(context, program);
    context_clear(context);
    TRACE_DEBUG("end");

    return 0;
}
