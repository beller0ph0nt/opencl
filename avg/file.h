#ifndef __FILE_H
#define __FILE_H

int file_len(const char* fname, int* len);
int file_read(const char* fname, const int len, char* src);

#endif // __FILE_H
