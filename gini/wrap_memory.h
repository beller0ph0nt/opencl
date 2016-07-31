#ifndef __WRAP_STDLIB_H
#define __WRAP_STDLIB_H

#include <stdlib.h>

void*
_wrp_malloc(size_t size);

void
_w_free(void *ptr);

#endif // __WRAP_STDLIB_H
