#ifndef __WRAP_FILE_H
#define __WRAP_FILE_H

#include <stdio.h>

FILE* _wrp_fopen(const char *path, const char *mode);

void _wrp_fclose(FILE *fd);

long _wrp_ftell(FILE *fd);

void _wrp_fseek(FILE *fd, long offset, int whence);

void _wrp_fread(void *ptr, size_t size, size_t nmemb, FILE *stream);

#endif // __WRAP_FILE_H
