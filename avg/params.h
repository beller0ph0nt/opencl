#ifndef __PARAMS_H
#define __PARAMS_H

#define FLOAT(x)            XFLOAT(x)
#define XFLOAT(x)           float ## x

#define DOUBLE(x)           XDOUBLE(x)
#define XDOUBLE(x)          double ## x

#define TO_STR(s)           XTO_STR(s)
#define XTO_STR(s)          #s

#endif // __PARAMS_H
