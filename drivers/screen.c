// screen.c
// author: stophin
//
#include "../drivers/screen.h"
#include "../lib/lib.h"
#include "../res/resource.h"

BOOTINFO bootinfo;

INT vga_test() {
	
	if (bootinfo.vmode > 8) {

		int *addr = (int *)bootinfo.vram;
		//int *addr = (int *)0xffff800000a00000;
		int i;


		for(i = 0 ;i<bootinfo.scrnx*20;i++)
		{
			*((char *)addr+0)=(char)0x00;
			*((char *)addr+1)=(char)0x00;
			*((char *)addr+2)=(char)0xff;
			*((char *)addr+3)=(char)0x00;	
			addr +=1;	
		}
		for(i = 0 ;i<bootinfo.scrnx*20;i++)
		{
			*((char *)addr+0)=(char)0x00;
			*((char *)addr+1)=(char)0xff;
			*((char *)addr+2)=(char)0x00;
			*((char *)addr+3)=(char)0x00;	
			addr +=1;	
		}
		for(i = 0 ;i<bootinfo.scrnx*20;i++)
		{
			*((char *)addr+0)=(char)0xff;
			*((char *)addr+1)=(char)0x00;
			*((char *)addr+2)=(char)0x00;
			*((char *)addr+3)=(char)0x00;	
			addr +=1;	
		}
		for(i = 0 ;i<bootinfo.scrnx*20;i++)
		{
			*((char *)addr+0)=(char)0xff;
			*((char *)addr+1)=(char)0xff;
			*((char *)addr+2)=(char)0xff;
			*((char *)addr+3)=(char)0x00;	
			addr +=1;	
		}

		while(1)
			;
	} 
		
	INT i;
	for (i = 0; i <= 0xffff; i++) {
		write_mem(bootinfo.vram + i, i & 0xf);
	}
	
	INT x = bootinfo.scrnx;
	INT y = bootinfo.scrny;
	boxfill(COL_008484,  0,         0, x, y);
	boxfill(COL_C6C6C6,  0,     	y, x, y);
	//boxfill(COL_C6C6C6,  0,     y - 28, x -  1, y - 28);
	/*
	boxfill(COL_FFFFFF,  0,     y - 27, x -  1, y - 27);
	boxfill(COL_C6C6C6,  0,     y - 26, x -  1, y -  1);

	boxfill(COL_FFFFFF,  3,     y - 24, 59,     y - 24);
	boxfill(COL_FFFFFF,  2,     y - 24,  2,     y -  4);
	boxfill(COL_848484,  3,     y -  4, 59,     y -  4);
	boxfill(COL_848484, 59,     y - 23, 59,     y -  5);
	boxfill(COL_000000,  2,     y -  3, 59,     y -  3);
	boxfill(COL_000000, 60,     y - 24, 60,     y -  3);

	boxfill(COL_848484, x - 47, y - 24, x -  4, y - 24);
	boxfill(COL_848484, x - 47, y - 23, x - 47, y -  4);
	boxfill(COL_FFFFFF, x - 47, y -  3, x -  4, y -  3);
	boxfill(COL_FFFFFF, x -  3, y - 24, x -  3, y -  3);
	
	boxfill(COL_FF0000, 20, 20, 120, 120);
	boxfill(COL_00FF00, 70, 50, 170, 150);
	boxfill(COL_0000FF, 120, 80, 220, 180);
	*/
	
	/*
	i = 0;
	putfont(10 + FONT_W * i++, 10, COL_FFFFFF, 1, sys_font + 'A' * FONT_H);
	putfont(10 + FONT_W * i++, 10, COL_FFFFFF, 1, sys_font + 'B' * FONT_H);
	putfont(10 + FONT_W * i++, 10, COL_FFFFFF, 1, sys_font + 'C' * FONT_H);
	
	i = 1;
	putstring(10, 10 + FONT_H * i++, COL_FFFFFF, 1, "Hello OS!");
	*/
	
	draw_cursor(0, 0, COL_000000, COL_FFFFFF);
}

void draw_cursor(INT px, INT py, BYTE bc, BYTE dc) {
	INT x, y;
	for (y = 0; y < 16; y++) {
		for (x = 0; x < 10; x++) {
			if (cursor[y][x] == '1') {
				bootinfo.vram[(py + y) * bootinfo.scrnx + px + x] = bc;
			}
			else if (cursor[y][x] == '2') {
				bootinfo.vram[(py + y) * bootinfo.scrnx + px + x] = dc;
			}
			else {
			}
		}
	}
}

void boxfill(BYTE c, INT x0, INT y0, INT x1, INT y1) {
	INT x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++) {
			bootinfo.vram[y * bootinfo.scrnx + x] = c;
		}
	}
}

void putfont(INT x, INT y, BYTE c, BYTE scl, BYTE *font)
{
	INT i, j, k, l;
	BYTE d;	/* data */
	BYTE *p;
	BYTE mask;
	for (i = 0; i < FONT_H; i++) {
		d = font[i];
		mask = FONT_M;
		for (j = 0; j < FONT_W; j++) {
			p =  bootinfo.vram + (y + i * scl) * bootinfo.scrnx + x + j * scl;
			if ((d & mask) != 0) {
				for (k = 0; k < scl; k++) {
					for (l = 0; l < scl; l++) {
						p[k * bootinfo.scrnx + l] = c;
					}
				}
			}
			mask >>= 1;
		}
	}
	return;
}

void putstring(INT x, INT y, BYTE c, BYTE scl, BYTE * str) {
	INT i = 0;
	for (i = 0; str[i] != '\0'; i ++) {
		putfont(x + FONT_W * scl * i, y, c, scl, sys_font + str[i] * FONT_H);
	}
}


void init_palette() {
	// Get VGA settings 
	// previously defined in boot/vga_start.asm
	bootinfo = *(BOOTINFO *)VMODE;

	if (bootinfo.vmode > 8) {
		return;
	}

	static BYTE table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	
		0xff, 0x00, 0x00,	
		0x00, 0xff, 0x00,	
		0xff, 0xff, 0x00,	
		0x00, 0x00, 0xff,	
		0xff, 0x00, 0xff,	
		0x00, 0xff, 0xff,	
		0xff, 0xff, 0xff,	
		0xc6, 0xc6, 0xc6,	
		0x84, 0x00, 0x00,	
		0x00, 0x84, 0x00,	
		0x84, 0x84, 0x00,	
		0x00, 0x00, 0x84,	
		0x84, 0x00, 0x84,	
		0x00, 0x84, 0x84,	
		0x84, 0x84, 0x84	
	};
	
	set_palette(0, 15, table_rgb);
	
	return;
}

void set_palette(INT start, INT end, BYTE *rgb)
{
	INT i, eflags;
	eflags = io_load_eflags();
	io_cli(); 					
	port_byte_out(0x03c8, start);
	for (i = start; i <= end; i++) {
		port_byte_out(0x03c9, rgb[0] / 4);
		port_byte_out(0x03c9, rgb[1] / 4);
		port_byte_out(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	
	return;
}


void putfont_b(BYTE * buf, INT width, INT x, INT y, BYTE c, BYTE scl, BYTE *font)
{
	INT i, j, k, l;
	BYTE d;	/* data */
	BYTE *p;
	BYTE mask;
	for (i = 0; i < FONT_H; i++) {
		d = font[i];
		mask = FONT_M;
		for (j = 0; j < FONT_W; j++) {
			p =  buf + (y + i * scl) * width + x + j * scl;
			if ((d & mask) != 0) {
				for (k = 0; k < scl; k++) {
					for (l = 0; l < scl; l++) {
						p[k * width + l] = c;
					}
				}
			}
			mask >>= 1;
		}
	}
	return;
}
void boxfill_b(BYTE * buf, INT width, BYTE c, INT x0, INT y0, INT x1, INT y1) {
	INT x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++) {
			buf[y * width + x] = c;
		}
	}
}

void putstring_b(BYTE * buf, INT width, INT x, INT y, BYTE c, BYTE scl, BYTE * str) {
	INT i = 0;
	for (i = 0; str[i] != '\0'; i ++) {
		boxfill_b(buf, width, COL_0000FF, x + FONT_W * scl * i, y, x + FONT_W * scl * (i + 1), y + FONT_H * scl);
		putfont_b(buf, width, x + FONT_W * scl * i, y, c, scl, sys_font + str[i] * FONT_H);
	}
}

void draw_cursor_b(BYTE * buf, INT width, INT px, INT py, BYTE bc, BYTE dc, BYTE tc) {
	INT x, y;
	for (y = 0; y < 16; y++) {
		for (x = 0; x < 10; x++) {
			if (cursor[y][x] == '1') {
				buf[(py + y) * width + px + x] = bc;
			}
			else if (cursor[y][x] == '2') {
				buf[(py + y) * width  + px + x] = dc;
			}
			else {
                buf[(py + y) * width  + px + x] = tc;
			}
		}
	}
}

void sprintf(INT x, INT y, BYTE c, BYTE b, BYTE scl, BYTE * str) {
	INT i = 0;
	for (i = 0; str[i] != '\0'; i ++) {
		boxfill_b(bootinfo.vram, bootinfo.scrnx, b, x + FONT_W * scl * i, y, x + FONT_W * scl * (i + 1), y + FONT_H * scl);
		putfont_b(bootinfo.vram, bootinfo.scrnx, x + FONT_W * scl * i, y, c, scl, sys_font + str[i] * FONT_H);
	}
}
