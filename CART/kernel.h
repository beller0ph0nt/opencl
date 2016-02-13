#ifndef __KERNEL_H
#define __KERNEL_H

#ifdef __cplusplus
extern "C" {
#endif

int kernel_len(const char * fname, int * len);
int kernel_read(const char * fname, const int len, char * src);

#ifdef __cplusplus
}
#endif

#endif // __KERNEL_H
