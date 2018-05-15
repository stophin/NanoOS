// lib.c
// author: stophin
//
#include "../lib/lib.h"
#include "../res/resource.h"
#include "../kernel/system_lib.h"

/**
 * C++ version 0.4 BYTE* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 *
 * changed by Stophin
 */
BYTE * itoa(BYTE* result, INT value, INT base) {
	BYTE * ptr = result, *ptr1 = result, tmp_BYTE;
	INT tmp_value;
	
	// check that the base if valid
	if (base < 2 || base > 36) { 
		*result = '\0'; 
		return result; 
	}


	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"\
			 [35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0) {
		*ptr++ = '-';
	}
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_BYTE = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_BYTE;
	}
	return result;
}

BYTE * strcat(BYTE * dest, const BYTE * src) {
	INT i,j;
	for (i = 0; dest[i] != '\0'; i++);
	for (j = 0; src[j] != '\0'; j++) {
		dest[i+j] = src[j];

	}
	dest[i+j] = '\0';
	return dest;
}

BYTE * memset(BYTE * buff, INT size, BYTE val) {
	INT i;
	for (i = 0; i < size; i ++) {
		buff[i] = val;
	}
	return buff;
}

BYTE * memcpy(BYTE * dest, const BYTE * src) {
	INT i;
	for (i = 0; src[i] != '\0'; i ++) {
		dest[i] = src[i];
	}
	dest[i] = '\0';
	return dest;
}

BYTE * memforce(BYTE * dest, const BYTE * src, int size) {
	INT i;
	for (i = 0; i < size; i ++) {
		dest[i] = src[i];
	}
	return dest;
}

void disp_shift(int lin) {
	io_cli();
	port_byte_out(0x3d4, 0xc);
	port_byte_out(0x3d5, ((80 * lin)>> 8) & 0xff);
	port_byte_out(0x3d4, 0xd);
	port_byte_out(0x3d5, (80 * lin) & 0xff);
	io_sti();
}

void disp_cur() {
	io_cli();
	port_byte_out(0x3d4, 0xe);
	port_byte_out(0x3d5, (((disp_pos - disp_base) / 2) >> 8) & 0xff);
	port_byte_out(0x3d4, 0xf);
	port_byte_out(0x3d5, ((disp_pos - disp_base) / 2) & 0xff);
	io_sti();
}

#include "../drivers/screen.h"

void disp_str(BYTE * str) {
	//boxfill(COL_FFFFFF, FONT_W * (disp_pos - disp_base), FONT_H * 5, FONT_W * (disp_pos - disp_base + 5), FONT_H * 6);
	putstring(FONT_W * 0, FONT_H * (disp_pos - disp_base), COL_000000, 1, str);
	disp_pos+= 1;
}
void disp_str_col(BYTE * str, BYTE col) {
	//boxfill(COL_FFFFFF, FONT_W * (disp_pos - disp_base), FONT_H * 6, FONT_W * (disp_pos - disp_base +5), FONT_H * 7);
	putstring(FONT_W * 0, FONT_H * (disp_pos - disp_base), COL_840000, 1, str);
	disp_pos+= 1;
}

void disp_int(int val) {
	itoa(buffer, val, 16);
	disp_str_col(buffer, 0x74);
}

BYTE * itoa_b(INT value, BYTE* result, INT base) {
	return itoa(result, value, base);
}