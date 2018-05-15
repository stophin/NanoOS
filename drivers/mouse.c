// mouse.c
// author: stophin
//
#include "../drivers/screen.h"
#include "../interrupt/interrupt.h"
#include "../kernel/system_call.h"
#include "../lib/lib.h"
#include "../res/resource.h"

#include "../drivers/mouse.h"
#include "../drivers/key.h"

#include "../tiny/OSTiny.h"

Mouse mouse;
// mouse interrupt service
void int_handler2c() {
	BYTE data;

	port_byte_out(PIC1_OCW2, 0x64);		//PIC1 IRQ-12 complete
	port_byte_out(PIC0_OCW2, 0x62);		//PIC0 IRQ-02 complete
	// need to take out the buffer or it will hang
	data = port_byte_in(0x0060);
	
	putstring(10, FONT_H * 1, COL_000000, 1, "Mouse");
	
	if (decode_mouse(data) == 1) {
		itoa(buffer, mouse.x, 10);
		itoa(buffer + 15, mouse.y, 10);
		strcat(buffer, ",");
		strcat(buffer, buffer + 15);

		boxfill(COL_FFFFFF, FONT_W * 10, FONT_H * 1, FONT_W * 20, FONT_H * 2);
		putstring(FONT_W * 10, FONT_H * 1, COL_000000, 1, buffer);
		
		draw_cursor(mouse.x, mouse.y, COL_000000, COL_FFFFFF);
	}
    taskFocus->MOUSE ++;
	
	return;
}

void init_mouse() {
	wait_sendready();
	port_byte_out(0x0064, 0xd4);
	wait_sendready();
	port_byte_out(0x0060, 0xf4);
	
	mouse.x = 0;
	mouse.y = 0;
	
	return;
}

int decode_mouse(BYTE data) {
	INT dx, dy;
	
	if (mouse.phase == 0) {
		if (data == 0xfa) {
			mouse.phase = 1;
		}
		return 0;
	}
	if (mouse.phase == 1) {
		if ((data & 0xc8) == 0x08) {
			mouse.buff[0] = data;
			mouse.phase = 2;
		}
		return 0;
	}
	if (mouse.phase == 2) {
		mouse.buff[1] = data;
		mouse.phase = 3;
		return 0;
	}

	if (mouse.phase == 3) {

		mouse.button = mouse.buff[0] & 0x07;

		mouse.buff[2] = data;
		mouse.phase = 1;

		dx = mouse.buff[1];
		dy = mouse.buff[2];
		if ((mouse.buff[0] & 0x10) != 0) {
			dx |= 0xffffff00;
		}
		if ((mouse.buff[0] & 0x20) != 0) {
			dy |= 0xffffff00;
		}
		dy = -dy;

		mouse.vx = dx;
		mouse.vy = dy;

		mouse.x += dx;
		mouse.y += dy;
		
		if (mouse.x < 0) {
			mouse.x = 0;
		}
		if (mouse.y < 0) {
			mouse.y = 0;
		}
		if (mouse.x >= bootinfo.scrnx) {
			mouse.x = bootinfo.scrnx;
		}
		if (mouse.y >= bootinfo.scrny) {
			mouse.y = bootinfo.scrny;
		}

		return 1;
	}
	return -1;
}