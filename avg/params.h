#ifndef __PARAMS_H
#define __PARAMS_H

//#define V_LEN               16

#define FLOAT(x)            XFLOAT(x)
#define XFLOAT(x)           float ## x

#define REAL(x)             XREAL(x)
#define XREAL(x)            double ## x

//#define AVG_PROG_NAME       avg
//#define AVG_PROG_NAME_STR   TO_STR(AVG_PROG_NAME)

#define TO_STR(s)           XTO_STR(s)
#define XTO_STR(s)          #s

#endif // __PARAMS_H
