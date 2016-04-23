#ifndef __FILE_H
#define __FILE_H

#ifdef __cplusplus
extern "C" {
#endif

int file_len(const char* fname, int* len);
int file_read(const char* fname, const int len, char* src);

#ifdef __cplusplus
}
#endif

#endif // __FILE_H
