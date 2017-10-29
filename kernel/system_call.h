// system_call.h
// author: stophin
//
// include system_call.asm's functions
// note these functions' addresses are needed in compile period
// if declared with extern, those necessarity will be put off till link peroid
extern void int_handler2c_asm();
extern void int_handler21_asm();
extern void int_handler20_asm();