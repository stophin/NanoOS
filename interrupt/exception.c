// exception.c
// author: stophin
//
#include "../res/resource.h"
#include "../lib/lib.h"
#include "../kernel/system_call.h"
#include "../kernel/system_exception.h"

#include "../interrupt/exception.h"

// exception handler
void exception_handler(INT vector_no, INT err_code, INT eip, INT cs, INT eflags) {
	INT i;

	int text_color = 0x74; /* red on gray */
	disp_str_col("\n", text_color);
	disp_str_col("Exception! --> ", text_color);
	disp_str_col(error_desc[vector_no], text_color);
	disp_str_col("\n", text_color);
	disp_str_col("EFLAGS:", text_color);
	disp_int(eflags);
	disp_str_col("\n", text_color);
	disp_str_col("CS:", text_color);
	disp_int(cs);
	disp_str_col("\n", text_color);
	disp_str_col("EIP:", text_color);
	disp_int(eip);
}

void init_exception(GATE_DESCRIPTOR *  idt) {
	// Interruption Gate(No Trap Gate)
	set_gatedesc(idt + INT_VECTOR_DIVIDE,   (DWORD)divide_error,		 1 * 8, 0x008e);
	set_gatedesc(idt + INT_VECTOR_DEBUG,		  (DWORD)single_step_exception,	 1 * 8, 0x008e);
	set_gatedesc(idt + INT_VECTOR_NMI,		  (DWORD)nmi,			 1 * 8, 0x008e);
	set_gatedesc(idt + INT_VECTOR_BREAKPOINT,	  (DWORD)breakpoint_exception, 1 * 8, 	0x008e);
	set_gatedesc(idt + INT_VECTOR_OVERFLOW,	  (DWORD)overflow,		1 * 8,	0x008e);
	set_gatedesc(idt + INT_VECTOR_BOUNDS,	  (DWORD)bounds_check,		 1 * 8, 0x008e);
	set_gatedesc(idt + INT_VECTOR_INVAL_OP,	  (DWORD)inval_opcode,		 1 * 8, 0x008e);
	set_gatedesc(idt + INT_VECTOR_COPROC_NOT,	  (DWORD)copr_not_available,	 1 * 8, 0x008e);
	set_gatedesc(idt + INT_VECTOR_DOUBLE_FAULT,	  (DWORD)double_fault,		 1 * 8, 0x008e);
	set_gatedesc(idt + INT_VECTOR_COPROC_SEG,	  (DWORD)copr_seg_overrun,		 1 * 8, 0x008e);
	set_gatedesc(idt + INT_VECTOR_INVAL_TSS,	  (DWORD)inval_tss,			 1 * 8, 0x008e);
	set_gatedesc(idt + INT_VECTOR_SEG_NOT,	  (DWORD)segment_not_present,	 1 * 8, 0x008e);
	set_gatedesc(idt + INT_VECTOR_STACK_FAULT,	  (DWORD)stack_exception,		 1 * 8, 0x008e);
	set_gatedesc(idt + INT_VECTOR_PROTECTION,	  (DWORD)general_protection,	 1 * 8, 0x008e);
	set_gatedesc(idt + INT_VECTOR_PAGE_FAULT,	  (DWORD)page_fault,		 1 * 8, 0x008e);
	set_gatedesc(idt + INT_VECTOR_COPROC_ERR,	  (DWORD)copr_error,		 1 * 8, 0x008e);
}