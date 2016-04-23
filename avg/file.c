#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "file.h"

int file_len(const char* fname, int* len)
{
    int ret = 0;

    struct stat st;

    if (stat(fname, &st) != -1)
    {
        *len = st.st_size;
    }
    else
    {
        ret = 1;
        perror("stat");
    }

    return ret;
}

int file_read(const char * fname, const int len, char * src)
{
    int ret = 0x00;

    int fd = open(fname, O_RDONLY);
    if (fd != -1)
    {
        if (read(fd, src, len) == -1)
        {
            ret |= 0x04;
            perror("kernel_read read error");
        }

        if (close(fd) == -1)
        {
            ret |= 0x02;
            perror("kernel_read close error");
        }
    }
    else
    {
        ret |= 0x01;
        perror("kernel_read open error");
    }

    return ret;
}
