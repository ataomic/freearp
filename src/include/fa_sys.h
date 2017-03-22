#ifndef FA_SYS_H
#define FA_SYS_H

#include <string.h>

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define FA_OK		0
#define FA_FAIL		-1
#define FA_DUP		-2

#define fa_int_cmp(a, b) ((int)(a)-(int)(b))

#endif

