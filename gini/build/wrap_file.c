#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "wrap_file.h"

FILE* _wrp_fopen(const char *path, const char *mode)
{
    FILE *fd = fopen(path, mode);

    if (fd == NULL)
    {
        exit(FILE_OPEN_ERROR);
    }

    return fd;
}

void _wrp_fclose(FILE *fd)
{
    if (fclose(fd) == EOF)
    {
        exit(FILE_CLOSE_ERROR);
    }
}

long _wrp_ftell(FILE *fd)
{
    long len = ftell(fd);

    if (len == -1)
    {
        exit(FILE_TELL_ERROR);
    }

    return len;
}

void _wrp_fseek(FILE *fd, long offset, int whence)
{
    if (fseek(fd, offset, whence) == -1)
    {
        exit(FILE_SEEK_ERROR);
    }
}

void _wrp_fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    if (fread(ptr, size, nmemb, stream) != nmemb)
    {
        exit(FILE_READ_ERROR);
    }
}
