// screen.h
// author: stophin
//
#include "../lib/type.h"

#ifndef _SCREEN_
#define _SCREEN_

// define colors
#define COL_000000		0
#define COL_FF0000		1
#define COL_00FF00		2
#define COL_FFFF00		3
#define COL_0000FF		4
#define COL_FF00FF		5
#define COL_00FFFF		6
#define COL_FFFFFF		7
#define COL_C6C6C6		8
#define COL_840000		9
#define COL_008400		10
#define COL_848400		11
#define COL_000084		12
#define COL_840084		13
#define COL_008484		14
#define COL_848484		15

// VGA memory used by SCRN_INFO
// previously defined in boot/vga_start.asm
#define VMODE	0x0ff0
// SCRN_INFO
typedef struct BOOTINFO {
	BYTE vmode;		// VGA mode
	WORD scrnx;		// screen X 
	WORD scrny;		// screen Y
	BYTE * vram;	// memory cache
} BOOTINFO;

extern BOOTINFO bootinfo;

#define FONT_W 6
#define FONT_H 12
#define FONT_M 0x40
extern BYTE sys_font[4096];

INT vga_test();
void init_palette();
void set_palette(INT start, INT end, BYTE *rgb);
void boxfill(BYTE c, INT x0, INT y0, INT x1, INT y1);
void putfont(INT x, INT y, BYTE c, BYTE scl, BYTE *font);
void putstring(INT x, INT y, BYTE c, BYTE scl, BYTE * str);
void draw_cursor(INT px, INT py, BYTE bc, BYTE dc);

void boxfill_b(BYTE * buf, INT width, BYTE c, INT x0, INT y0, INT x1, INT y1);
void putfont_b(BYTE * buf, INT width, INT x, INT y, BYTE c, BYTE scl, BYTE *font);
void putstring_b(BYTE * buf, INT width, INT x, INT y, BYTE c, BYTE scl, BYTE * str);
void draw_cursor_b(BYTE * buf, INT width, INT px, INT py, BYTE bc, BYTE dc);
void sprintf(INT x, INT y, BYTE c, BYTE b, BYTE scl, BYTE * str);
#endif