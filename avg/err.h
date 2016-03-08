#ifndef __ERR_H
#define __ERR_H

#define MAX_STR_ERR_LEN     255

#ifdef __cplusplus
extern "C" {
#endif

void err_to_str(const cl_uint err, char *str);

#ifdef __cplusplus
}
#endif

#endif // __ERR_H
