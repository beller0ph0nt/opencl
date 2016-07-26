#include <stdio.h>

#include "file.h"
#include "wrap_file.h"

long file_len(const char *path)
{
    FILE *fd = _wrp_fopen(path, "r");
    _wrp_fseek(fd, 0L, SEEK_END);
    long len = _wrp_ftell(fd);
    _wrp_fclose(fd);

    return len;
}

void file_read(const char *path, const long len, char *src)
{
    FILE *fd = _wrp_fopen(path, "r");
    _wrp_fread(src, 1, len, fd);
    _wrp_fclose(fd);
}
