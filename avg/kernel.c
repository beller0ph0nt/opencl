#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "kernel.h"

int kernel_len(const char * fname, int * len)
{
    int ret = 0x00;

    int fd = open(fname, O_RDONLY);
    if (fd != -1)
    {
        int offset = lseek(fd, 0, SEEK_END);
        if (offset != -1)
        {
            *len = offset;
        }
        else
        {
            ret |= 0x04;
            perror("kernel_len lseek error");
        }

        if (close(fd) == -1)
        {
            ret |= 0x02;
            perror("kernel_len close error");
        }
    }
    else
    {
        ret |= 0x01;
        perror("kernel_len open error");
    }

    return ret;
}

int kernel_read(const char * fname, const int len, char * src)
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
