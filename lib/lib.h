// lib.h
// author: stophin
//
#include "../lib/type.h"

#ifndef _LIB_
#define _LIB_

BYTE * itoa(BYTE* result, INT value, INT base);
BYTE * strcat(BYTE * dest, const BYTE * src);
BYTE * memset(BYTE * buff, INT size, BYTE val);
BYTE * memcpy(BYTE * dest, const BYTE * src);
BYTE * memforce(BYTE * dest, const BYTE * src, int size);


extern unsigned int disp_pos;
extern unsigned int disp_base;
void disp_shift(int lin);
void disp_str(BYTE * str);
void disp_str_col(BYTE * str, BYTE col);
void disp_int(int val);

BYTE * itoa_b(INT value, BYTE* result, INT base);
#endif