#include <stdlib.h>

#include "free.h"

void free_ptr_2d(void **p, const unsigned long int len)
{
    if (p == NULL)
    {
        return;
    }

    unsigned long int i;
    for (i = 0; i < len; i++)
    {
        free_ptr_1d(p[i]);
    }

    free(p);
}

void free_ptr_1d(void *p)
{
    if (p == NULL)
    {
        return;
    }

    free(p);
}
