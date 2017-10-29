// exception.h
// author: stophin
//
#include "../lib/type.h"
#include "../interrupt/interrupt.h"

/* Exception Interruption Vectors */
#define	INT_VECTOR_DIVIDE		0x0
#define	INT_VECTOR_DEBUG		0x1
#define	INT_VECTOR_NMI			0x2
#define	INT_VECTOR_BREAKPOINT		0x3
#define	INT_VECTOR_OVERFLOW		0x4
#define	INT_VECTOR_BOUNDS		0x5
#define	INT_VECTOR_INVAL_OP		0x6
#define	INT_VECTOR_COPROC_NOT		0x7
#define	INT_VECTOR_DOUBLE_FAULT		0x8
#define	INT_VECTOR_COPROC_SEG		0x9
#define	INT_VECTOR_INVAL_TSS		0xA
#define	INT_VECTOR_SEG_NOT		0xB
#define	INT_VECTOR_STACK_FAULT		0xC
#define	INT_VECTOR_PROTECTION		0xD
#define	INT_VECTOR_PAGE_FAULT		0xE
#define	INT_VECTOR_COPROC_ERR		0x10

/* Exception Interruption Handlers */
extern void divide_error();
extern void single_step_exception();
extern void nmi();
extern void breakpoint_exception();
extern void overflow();
extern void bounds_check();
extern void inval_opcode();
extern void copr_not_available();
extern void double_fault();
extern void copr_seg_overrun();
extern void inval_tss();
extern void segment_not_present();
extern void stack_exception();
extern void general_protection();
extern void page_fault();
extern void copr_error();

void exception_handler(INT vector_no, INT err_code, INT eip, INT cs, INT eflags);
void init_exception(GATE_DESCRIPTOR *  idt);