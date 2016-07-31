#include <stdlib.h>

#include "def.h"
#include "free.h"
#include "wrap_memory.h"

void
free_ptr_2d(void** p, const unsigned long int len)
{
    if (p == NULL)
        return;

    TRACE_DEBUG("begin");
    unsigned long int i;
    for (i = 0; i < len; i++)
        free_ptr_1d(p[i]);

    _w_free(p);
}

void
free_ptr_1d(void *p)
{
    TRACE_DEBUG("begin");
    _w_free(p);
}
