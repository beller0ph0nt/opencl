#include <stdlib.h>

void free_ptr_2d(void **p, const unsigned long int len_2d)
{
    unsigned long int i;

    if (p != NULL)
    {
        for (i = 0; i < len_2d; i++)
        {
            if (p[i] != NULL)
            {
                free(p[i]);
                p[i] = NULL;
            }
        }

        free(p);
        p = NULL;
    }
}

void free_ptr_1d(void *p)
{
    if (p != NULL)
    {
        free(p);
        p = NULL;
    }
}
