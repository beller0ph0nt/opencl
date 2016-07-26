#ifndef __FILE_H
#define __FILE_H

long file_len(const char *path);
void file_read(const char *path, const long len, char *src);

#endif // __FILE_H
