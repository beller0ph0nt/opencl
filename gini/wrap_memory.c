#include <string.h>

#include "def.h"
#include "errors.h"
#include "wrap_memory.h"

void*
_wrp_malloc(size_t size)
{
    TRACE_DEBUG("begin");

    void* p = malloc(size);
    if (p == NULL)
        exit(MEMORY_MALLOC_ERROR);

    memset(p, 0, size);

    return p;
}

void
_w_free(void* ptr)
{
    TRACE_DEBUG("begin");
    if (ptr == NULL)
        return;

    free(ptr);
}
