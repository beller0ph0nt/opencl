#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "wrap_memory.h"

void* _wrp_malloc(size_t size)
{
    void *p = malloc(size);

    if (p == NULL)
    {
        exit(MEMORY_MALLOC_ERROR);
    }

    memset(p, 0, size);

    return p;
}
