// resource.c
// author: stophin
//
#include "../res/resource.h"

BYTE mouse_buff[100];
BYTE key_buff[100];

BYTE buffer[100];
BYTE buffer_t[100];

BYTE error_desc[][100] = {
	"#DE Divide Error!!",
	"#DB RESERVED",
	"¡ª  NMI Interrupt",
	"#BP Breakpoint",
	"#OF Overflow",
	"#BR BOUND Range Exceeded",
	"#UD Invalid Opcode (Undefined Opcode)",
	"#NM Device Not Available (No Math Coprocessor)",
	"#DF Double Fault",
	"    Coprocessor Segment Overrun (reserved)",
	"#TS Invalid TSS",
	"#NP Segment Not Present",
	"#SS Stack-Segment Fault",
	"#GP General Protection",
	"#PF Page Fault",
	"¡ª  (Intel reserved. Do not use.)",
	"#MF x87 FPU Floating-Point Error (Math Fault)",
	"#AC Alignment Check",
	"#MC Machine Check",
	"#XF SIMD Floating-Point Exception"
};

BYTE table_rgb[16 * 3] = {
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

BYTE  cursor[17][11] =
{
	"1..........",
	"11.........",
	"121........",
	"1221.......",
	"12221......",
	"122221.....",
	"1222221....",
	"12222221...",
	"122222221..",
	"1222222221.",
	"12222211111",
	"1221221....",
	"121.1221...",
	"11..1221...",
	"1....1221..",
	".....1221..",
	"......11..."
};

BYTE ascii_tab[58] = { 0, 0 /* ESC (1) */, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
						'-', '^', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 
						'@', '[', '\n', 0 /* L-CTRL (29) */, 'a', 's', 'd', 'f', 'g', 'h', 'j', 
						'k', 'l', ';', ':', 0 /* (41) */, 0 /* L-SHIFT (42) */, ']', 'z', 'x', 
						'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0 /* R-SHIFT (54) */, 0 /* ?  (55) */, 
						0 /* L-ALT (56) */, ' ' 
					}; 

BYTE ascii_shift_tab[58] = { 0, 0, '!', '"', '#', '$', '%', '&', '\'', '(', ')', 0, '=', '~', 0, 
							0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0, 0, 
							'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0, 0, '}', 'Z',
							 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 0, 0, 0
						};