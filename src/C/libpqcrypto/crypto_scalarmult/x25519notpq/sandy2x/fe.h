/*
   This file is adapted from ref10/fe.h:
   All the redundant functions are removed.
*/

#ifndef FE_H
#define FE_H

#include "crypto_uint64.h"

typedef crypto_uint64 fe[10];

/*
fe means field element.
Here the field is \Z/(2^255-19).
An element t, entries t[0]...t[9], represents the integer
t[0]+2^26 t[1]+2^51 t[2]+2^77 t[3]+2^102 t[4]+...+2^230 t[9].
Bounds on each t[i] vary depending on context.
*/

extern void fe_frombytes(fe, const unsigned char *);

#endif
