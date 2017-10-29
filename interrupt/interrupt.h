// interrupt.h
// author: stophin
//
#include "../lib/type.h"

#define ADR_GDT	0x270000
#define ADR_IDT	0x26f800

// PIC ports
#define PIC0_ICW1 0x0020
#define PIC0_OCW2 0x0020
#define PIC0_IMR  0x0021
#define PIC0_ICW2 0x0021
#define PIC0_ICW3 0x0021
#define PIC0_ICW4 0x0021
#define PIC1_ICW1 0x00a0
#define PIC1_OCW2 0x00a0
#define PIC1_IMR  0x00a1
#define PIC1_ICW2 0x00a1
#define PIC1_ICW3 0x00a1
#define PIC1_ICW4 0x00a1

#ifndef _INTERRUPT_
#define _INTERRUPT_

// GDT
typedef struct SEGM_DESCRIPTOR {
	WORD	limit_low, base_low;
	BYTE	base_mid, access_right;
	BYTE	limit_hight, base_high;
}SEGM_DESCRIPTOR;

// IDT
typedef struct GATE_DESCRIPTOR {
	WORD	offset_low, selector;
	BYTE	dw_count, access_right;
	WORD	offset_high;
}GATE_DESCRIPTOR;

void init_pic();
void init_gdtidt();
void set_segmdesc(SEGM_DESCRIPTOR * sd, DWORD limit, DWORD base, DWORD ar);
void set_gatedesc(GATE_DESCRIPTOR * gd, DWORD offset, DWORD selector, DWORD ar);

void exception_handler(INT vector_no, INT err_code, INT eip, INT cs, INT eflags);

#endif