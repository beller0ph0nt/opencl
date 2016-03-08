#ifndef __FREE_H
#define __FREE_H

#ifdef __cplusplus
extern "C" {
#endif

void free_ptr_1d(void *p);
void free_ptr_2d(void **p, const unsigned long int len_2d);

#ifdef __cplusplus
}
#endif

#endif // __FREE_H
