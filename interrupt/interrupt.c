// interrupt.c
// author: stophin
//
#include "../res/resource.h"
#include "../kernel/low_level.h"
#include "../kernel/system_call.h"
#include "../kernel/system_exception.h"
#include "../interrupt/exception.h"

#include "../interrupt/interrupt.h"


/*
* INT 00-0x0f was reserved for system interrupts
*/
void init_pic() {
	port_byte_out(PIC0_IMR, 0xff); // disable all INT from PIC0
	port_byte_out(PIC1_IMR, 0xff); // disable all INT from PIC1
	
	// PIC0 control register world
	port_byte_out(PIC0_ICW1, 0x11); // edge trigger mode
	port_byte_out(PIC0_ICW2, 0x20); // IRQ0-7 -> INT20-27
	port_byte_out(PIC0_ICW3, 1<<2); // PIC1 -> IRQ2
	port_byte_out(PIC0_ICW4, 0x01); // no cache
	
	// PIC1 control register world
	port_byte_out(PIC1_ICW1, 0x11); // edge trigger mode
	port_byte_out(PIC1_ICW2, 0x28); // IRQ8-15 -> INT28-2f
	port_byte_out(PIC1_ICW3, 2   ); // PIC1 -> IRQ2
	port_byte_out(PIC1_ICW4, 0x01); // no cache
	
	// all interruption was disabled here
	port_byte_out(PIC0_IMR, 0xff); // Disable all INT except PIC1
	// interrupt will be enabled in kernel
	port_byte_out(PIC1_IMR, 0xff); // disable all INT from PIC1
	
	return;
}

void init_gdtidt() {
	SEGM_DESCRIPTOR * gdt = (SEGM_DESCRIPTOR *) ADR_GDT;
	GATE_DESCRIPTOR * idt = (GATE_DESCRIPTOR *) ADR_IDT;
	INT i;
	
	// GDT: 2^13 = 8192
	for (i = 0; i < 8192; i ++) {
		set_segmdesc(gdt + i, 0, 0, 0);
	}
	
	// here we set 0x08 as code segment and 0x0f as data segement
	// just as same as that in boot.asm so that we do not need to
	// change ds, es, ss, fs, gs for convinences' sake

	// code segment has the same base as that in boot.asm
	// and expands at an available space limit
	set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x409a);//9a readable/executable code
	// data segment including all the 4G area
	set_segmdesc(gdt + 2, 0xffffffff, 0x00000000, 0x4092);//92 readable/writable data
	// limit: 0xffff = 8192 * 8 - 1
	load_gdtr(0xffff, gdt);
	
	// IDT: 256
	for (i = 0; i < 256; i++) {
		set_gatedesc(idt + i, 0, 0, 0);
	}
	// key IRQ 0x21
	set_gatedesc(idt + 0x21, (DWORD)int_handler21_asm, 1 * 8, 0x008e);//8e interrupt gate
	// mouse IRQ 0x2c
	set_gatedesc(idt + 0x2c, (DWORD)int_handler2c_asm, 1 * 8, 0x008e);//8e interrupt gate
	// timer IRQ 0x00
	set_gatedesc(idt + 0x20, (DWORD)int_handler20_asm, 1 * 8, 0x008e);//8e interrupt gate
	// Call Gate
	set_gatedesc(idt + 0x30, (DWORD)int_handler30_asm, 1 * 8, 0x008e + 0x60);//8e interrupt gate
	// initialize exceptions
	init_exception(idt);
	// limit: 0x7ff = 256 * 8 - 1
	load_idtr(0x7ff, idt);
	
	return;
}

void set_segmdesc(SEGM_DESCRIPTOR * sd, DWORD limit, DWORD base, DWORD ar) {
	if (limit > 0xfffff) {
		ar |= 0x8000;	 //G bit = 1
		limit /= 0x1000;
	}
	sd->limit_low	= limit & 0xffff;
	sd->base_low	= base  & 0xffff;
	sd->base_mid	= (base >> 16) & 0xff;
	sd->access_right= ar & 0xff;
	sd->limit_hight	= ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high	= (base >> 24) & 0xff;
	
	return;
}

void set_gatedesc(GATE_DESCRIPTOR * gd, DWORD offset, DWORD selector, DWORD ar) {
	gd->offset_low	= offset & 0xffff;
	gd->selector	= selector;
	gd->dw_count	= (ar >> 8) & 0xff;
	gd->access_right= ar & 0xff;
	gd->offset_high	= (offset >> 16) & 0xffff;
	
	return;
}