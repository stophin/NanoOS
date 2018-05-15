// key.c
// author: stophin
//
#include "../drivers/screen.h"
#include "../interrupt/interrupt.h"
#include "../kernel/system_call.h"
#include "../res/resource.h"

#include "../drivers/key.h"

Key key = {0, ascii_tab, ascii_shift_tab};

#include "../tiny/OSTiny.h"

// Key interrupt service
void int_handler21() {
	BYTE data;
	
	port_byte_out(PIC0_OCW2, 0x61);		//PIC0 IRQ-01 complete
	// need to take out the buffer or it will hang
	data = port_byte_in(0x0060);
	
	putstring(10, FONT_H * 0, COL_000000, 1, "Key");
	
	data = decode_key(data);
	
	boxfill(COL_FFFFFF, FONT_W * 10, FONT_H * 0, FONT_W * 11, FONT_H * 1);
	putfont(FONT_W * 10, FONT_H * 0, COL_000000, 1, sys_font + data * FONT_H);

	taskFocus->key.put(&taskFocus->key, data);
	
	return;
}

void wait_sendready() {
	for (;;) {
		if ((port_byte_in(0x0064) & 0x02) == 0) {
			break;
		}
	}
	
	return;
}

void init_key() {
	wait_sendready();
	port_byte_out(0x0064, 0x60);
	wait_sendready();
	port_byte_out(0x0060, 0x47);

	return;
}

BYTE decode_key(BYTE data) {
	if (data >= 0 && data < 58) {
		return key.ascii_tab[data];
	}
	return 0;
}
