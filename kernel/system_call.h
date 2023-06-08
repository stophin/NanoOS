// system_call.h
// author: stophin
//
// include system_call.asm's functions
// note these functions' addresses are needed in compile period
// if declared with extern, those necessarity will be put off till link peroid
extern void int_handler2c_asm();
extern void int_handler21_asm();
extern void int_handler20_asm();
extern void int_handler30_asm();

extern void io_hlt(void);
extern void io_cli(void);
extern void io_sti(void);
extern void io_stihlt(void);
extern void io_nop(void);
extern void io_store_eflags(int);
extern int io_load_eflags(void);
extern void store_cr0(int);
extern int load_cr0(void);
extern void load_gdtr(int limit, void* address);
extern void load_idtr(int limit, void* address);
extern void load_ldtr(int ltr);
extern void task_switch4();
extern void task_switch3();
extern void far_jump(int eip, int cs);
extern void write_mem(void* addr, int data);
extern int check_mem(int start, int end);