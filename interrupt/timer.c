// timer.c
// author: stophin
//
#include "../kernel/system_call.h"
#include "../interrupt/interrupt.h"
#include "../drivers/screen.h"
#include "../res/resource.h"

#include "../interrupt/timer.h"

#include "../tiny/OSTiny.h"

TIMER_CTL timer_ctl;

void int_handler20(DWORD *esp) {
	io_cli();
	

	port_byte_out(PIC0_OCW2, 0x60);		//PIC0 IRQ-00 complete

	timer_ctl.totalCount ++;
	timer_ctl.counter ++;

	itoa(buffer_t, timer_ctl.counter, 10);
	boxfill(COL_FFFFFF, FONT_W * 20, FONT_H * 2, FONT_W * 30, FONT_H * 3);
	putstring(FONT_W * 20, FONT_H * 2, COL_000000, 1, buffer_t);
	
	// Switch task every milliseconds
	if (timer_ctl.counter >= 1) {
		timer_ctl.counter = 0;
		OSTickISR();
		memcpy(buffer_t, "Task:");
		itoa(buffer_t + 5, taskCur->offset, 10);
		boxfill(COL_FFFFFF, FONT_W * 20, FONT_H * 3, FONT_W * 30, FONT_H * 4);
		putstring(FONT_W * 20, FONT_H * 3, COL_000000, 1, buffer_t);
	}
	
	io_sti();

	return;
}

void init_pit() {
	INT i;
	port_byte_out(PIT_CTRL, 0x34); 	//
	port_byte_out(PIT_CNT0, 0x9c);	//
	port_byte_out(PIT_CNT0, 0x2e);	//
	
	timer_ctl.counter = 0;
	timer_ctl.totalCount = 0;

	return;
}