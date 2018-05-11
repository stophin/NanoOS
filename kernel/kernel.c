// kernel.c
// author: stophin
//
#include "../kernel/system_exception.h"
#include "../drivers/screen.h"
#include "../interrupt/interrupt.h"
#include "../interrupt/timer.h"
#include "../drivers/key.h"
#include "../drivers/mouse.h"
#include "../drivers/layer.h"
#include "../res/resource.h"

#include "../tiny/OSTiny.h"

#define INTERRUPT(mode) //(mode? (DISABLE_INTERRUPT()):(ENABLE_INTERRUPT()))

LAYER_CTL * layer_ctl;

void Initialize() {
	int i = 0;
	
	init_palette();
	vga_test();
	
	// We setted gdt in boot.asm temporarily aiming to load
	// kernel successfully, while in kernel we need to 
	// re-configure gdt, and including idt
	init_gdtidt();
	putstring(0, FONT_H * i ++, COL_FFFFFF, 1, "Load gdt/idt ok!");
	
	init_pit();
	putstring(0, FONT_H * i ++, COL_FFFFFF, 1, "Set pit ok!");
	
	init_pic();
	putstring(0, FONT_H * i ++, COL_FFFFFF, 1, "Set pic ok!");
	io_sti();
	putstring(0, FONT_H * i ++, COL_FFFFFF, 1, "Set sti ok!");
	
	init_key();
	putstring(0, FONT_H * i ++, COL_FFFFFF, 1, "Init key ok!");
	
	init_mouse();
	putstring(0, FONT_H * i ++, COL_FFFFFF, 1, "Init mouse ok!");
	
	// 0xf9: Enable key and mouse interruption
	// 0xf8: Enable key and mouse and timer interruption
	port_byte_out(PIC0_IMR, 0xf8);
	port_byte_out(PIC1_IMR, 0xef);
	
	putstring(0, FONT_H * i ++, COL_FFFFFF, 1, "All ok!");
}

void task3(void *pParam, void *lParam) {
	char c;

	INTERRUPT(1);
	//parameter address
	itoa(buffer, (int)pParam, 16);
	putstring(FONT_W * 10, FONT_H * 12, COL_000000, 1, buffer);

	//parameter value
	memcpy(buffer, pParam);
	//boxfill(COL_FFFFFF, FONT_W * (20 + *(i
	putstring(FONT_W * 20, FONT_H * 12, COL_000000, 1, buffer);
	INTERRUPT(0);
	INTERRUPT(1);
	//parameter value
	itoa(buffer, ((int)lParam) % 16, 16);//nt *)lParam), FONT_H * 13, FONT_W * 30, FONT_H * 14);
	putstring(FONT_W * (0 + (int)lParam), FONT_H * 13, COL_000000, 1, buffer);

	INTERRUPT(0);
	INTERRUPT(1);
	//parameter address
	itoa(buffer, (int)taskCur->OSStackTop, 16);
	boxfill(COL_FFFFFF, FONT_W * 10, FONT_H * 14, FONT_W * 25, FONT_H * 15);
	putstring(FONT_W * 10, FONT_H * 14, COL_000000, 1, buffer);

	INTERRUPT(0);

	//call parameter as code
	itoa(buffer, ((int (*)())pParam)(), 16);
	putstring(FONT_W * 45, FONT_H * 12, COL_000000, 1, buffer);
	int i = 0;
	putstring(FONT_W * i ++, FONT_H * 12, COL_FFFFFF, 1, "in taks3");

	for (;;) {
		//io_cli();

		INTERRUPT(1);
		itoa(buffer, taskCur->runtime, 10);
		boxfill(COL_FFFFFF, FONT_W * 30, FONT_H * 12, FONT_W * 40, FONT_H * 13);
		putstring(FONT_W * 30, FONT_H * 12, COL_000000, 1, buffer);

		if (taskCur->key.linkcount > 0) {
			c = taskCur->key.pop(&taskCur->key);
			//io_sti();
			if (c) {
				putfont(FONT_W * i ++, FONT_H * 11, COL_000000, 1, sys_font + c * FONT_H);
			}
		} else {
			//io_stihlt();
		}

		INTERRUPT(1);
		taskControl.remove(&taskControl, taskCur);
		INTERRUPT(0);
		OSTimeDelay(100);
		// Task can be removed and recovered by other task code
		// and switch to other task
		// but can never return!!!
	}
}

void task2(void *pParam, void *lParam) {
	char c;

	INTERRUPT(1);
	//parameter address
	itoa(buffer, (int)pParam, 16);
	putstring(FONT_W * 10, FONT_H * 11, COL_000000, 1, buffer);

	//parameter value
	memcpy(buffer, pParam);
	putstring(FONT_W * 20, FONT_H * 11, COL_000000, 1, buffer);

	memMan.free(&memMan, (MEM_ADDR)pParam);
	INTERRUPT(0);

	BYTE * code = (BYTE *)memMan.alloc(&memMan, 1000);
	// int code() { return 0xa;}
	memforce(code, "\x55\x89\xE5\xB8\x0A\x00\x00\x00\x5D\xC3\x00", 11);

	int i = 0;
	putstring(FONT_W * i ++, FONT_H * 11, COL_FFFFFF, 1, "in taks2");
	OSTask * task;
	int count = 0;
	for (;;) {
		//io_cli();

		INTERRUPT(1);
		itoa(buffer, taskCur->runtime, 10);
		boxfill(COL_FFFFFF, FONT_W * 30, FONT_H * 11, FONT_W * 40, FONT_H * 12);
		putstring(FONT_W * 30, FONT_H * 11, COL_000000, 1, buffer);

		if (taskCur->key.linkcount > 0) {
			c = taskCur->key.pop(&taskCur->key);
			//io_sti();
			if (c) {
				putfont(FONT_W * i ++, FONT_H * 11, COL_000000, 1, sys_font + c * FONT_H);
				if (c == '2') {
					putstring(FONT_W * 25, FONT_H * 11, COL_000000, 1, "U2");
					trigger_ud2();
				}
				if (c == 'c') {
					task = taskControl.create(&taskControl, task3, code, (void *)count, TASK_STK_SIZE, 4);
					count ++;
					if (task) 
					{
						// Deactivate task
						taskLink.activate(&taskLink, task);
						putstring(FONT_W * 30, FONT_H * 11, COL_FFFFFF, 1, "task3 created, press b");
					} else {
						putstring(FONT_W * 30, FONT_H * 11, COL_FFFFFF, 1, "task3 error");
					}
				}
			}
		} else {
			//io_stihlt();
		}
		INTERRUPT(0);

		OSTimeDelay(100);
	}
}


void task1(void *pParam, void *lParam) {
	char c;
	LAYER * layer_back, * layer_mouse, * layer_win;
	BYTE * buf_back, * buf_mouse, * buf_win;

	int i = 0;
	INT mem_size = mem_test(0x00400000, 0xbfffffff);
	// create layer
	layer_ctl = layer_ctl_init(&memman, bootinfo.vram, bootinfo.scrnx, bootinfo.scrny);
	layer_back = layer_alloc(layer_ctl);
	layer_mouse = layer_alloc(layer_ctl);
	layer_win = layer_alloc(layer_ctl);

	// memory alloc and free test
	buf_back = (BYTE *) memman_alloc_4k(&memman, bootinfo.scrnx * bootinfo.scrny);
	buf_mouse = (BYTE *) memman_alloc_4k(&memman, 11*17);
	memman_free_4k(&memman, (DWORD)buf_back, bootinfo.scrnx * bootinfo.scrny);
	buf_back = (BYTE *) memman_alloc_4k(&memman, bootinfo.scrnx * bootinfo.scrny * 2);
	buf_win = (BYTE *) memman_alloc_4k(&memman, 160 * 80);

	// set layer
	layer_set(layer_back, buf_back, bootinfo.scrnx, bootinfo.scrny, 0);
	layer_set(layer_mouse, buf_mouse, 11, 17, 0);
	layer_set(layer_win, buf_win, 160, 80, 0);

	draw_back(layer_back);
	draw_window(layer_win);
	draw_cursor_b(buf_mouse, layer_mouse->bxsize, 0 , 0, COL_840000, COL_FFFFFF);

	// set depth height
	layer_height(layer_ctl, layer_back, 0);
	layer_height(layer_ctl, layer_mouse, 2);
	layer_height(layer_ctl, layer_win, 1);

	// set position
	layer_slide(layer_ctl, layer_win, 100, 100);

	i = 0;
	memcpy(buffer, "SCRN:");
	itoa_b(bootinfo.scrnx, buffer + 30, 10);
	strcat(buffer, buffer + 30);
	strcat(buffer, "x");
	itoa_b(bootinfo.scrny, buffer + 30, 10);
	strcat(buffer, buffer + 30);
	layer_string(layer_ctl, layer_back, 0, i ++ * FONT_H, COL_848484, COL_FFFFFF, 1, buffer);

	memcpy(buffer, "Memory:");
	itoa_b(mem_size / 1024, buffer + 30, 10);
	strcat(buffer, buffer + 30);
	strcat(buffer, " KB");
	layer_string(layer_ctl, layer_back, 0, i ++ * FONT_H, COL_848484, COL_FFFFFF, 1, buffer);

	memcpy(buffer, "buf_back : ");
	itoa_b((INT)buf_back, buffer + 30, 16);
	strcat(buffer, buffer + 30);
	layer_string(layer_ctl, layer_back, 0, i ++ * FONT_H, COL_848484, COL_FFFFFF, 1, buffer);
	memcpy(buffer, "buf_mouse: ");
	itoa_b((INT)buf_mouse, buffer + 30, 16);
	strcat(buffer, buffer + 30);
	layer_string(layer_ctl, layer_back, 0, i ++ * FONT_H, COL_848484, COL_FFFFFF, 1, buffer);
	memcpy(buffer, "buf_win  : ");
	itoa_b((INT)buf_win, buffer + 30, 16);
	strcat(buffer, buffer + 30);
	layer_string(layer_ctl, layer_back, 0, i ++ * FONT_H, COL_848484, COL_FFFFFF, 1, buffer);

	i = 0;
	putstring(FONT_W * i ++, FONT_H * 10, COL_FFFFFF, 1, "in taks1");
	int counter = 0;
	for (;;) {
		//io_cli();
		itoa_b(timer_ctl.totalCount, buffer, 10);
		layer_string(layer_ctl, layer_win, 10, 2 * FONT_H, COL_848484, COL_FFFFFF, 2, buffer);

		INTERRUPT(1);
		itoa(buffer, taskCur->runtime, 10);
		boxfill(COL_FFFFFF, FONT_W * 30, FONT_H * 10, FONT_W * 40, FONT_H * 11);
		putstring(FONT_W * 30, FONT_H * 10, COL_000000, 1, buffer);

		if (taskCur->MOUSE > 0) {
			taskCur->MOUSE --;

			if (mouse.x <= 0) {
				mouse.vx = 0;
			}
			if (mouse.y <= 0) {
				mouse.vy = 0;
			}
			if (mouse.x > bootinfo.scrnx - 1) {
				mouse.x = bootinfo.scrnx - 1;
				mouse.vx = 0;
			}
			if (mouse.y > bootinfo.scrny - 1) {
				mouse.y = bootinfo.scrny - 1;
				mouse.vy = 0;
			}
			if ((mouse.button & 0x01) != 0) {
				if (isin(layer_win, layer_mouse) == 1) {
					if (mouse.state == 0) {
						layer_win->mx0 = mouse.x - layer_win->vx0;
						layer_win->my0 = mouse.y - layer_win->vy0;
					}
					mouse.state = 1;
				}
			} else {
				mouse.state = 0;
			}
			if ((mouse.button & 0x02) != 0) {
			}
			if ((mouse.button & 0x04) != 0) {
			}
			DISABLE_INTERRUPT();
			//layer_slide(layer_ctl, layer_mouse, layer_mouse->vx0 + mouse.vx, layer_mouse->vy0 + mouse.vy);
			layer_slide(layer_ctl, layer_mouse, mouse.x, mouse.y);
			if (mouse.state == 1) {
				//layer_slide(layer_ctl, layer_win, layer_win->vx0 + mouse.vx, layer_win->vy0 + mouse.vy);
				//layer_slide(layer_ctl, layer_win, mouse.x, mouse.y);
				layer_slide(layer_ctl, layer_win, mouse.x - layer_win->mx0, mouse.y - layer_win->my0);
			}
			ENABLE_INTERRUPT();
		}
		if (taskCur->key.linkcount > 0) {
			c = taskCur->key.pop(&taskCur->key);
			//io_sti();
			if (c) {
				putfont(FONT_W * i ++, FONT_H * 10, COL_000000, 1, sys_font + c * FONT_H);
				buffer[0] = c;
				buffer[1] = '\0';
				layer_string(layer_ctl, layer_win, 10, FONT_H * 4, COL_848484, COL_FFFFFF, 2, buffer);
			}
		} else {
			//io_stihlt();
		}
		INTERRUPT(0);

		OSTimeDelay(100);
	}
}



void init(void * pParam,void * lParam) {
	char c;

	BYTE * code = (BYTE *)memMan.alloc(&memMan, 1000);
	memforce(code, "12345", 11);

	taskFocus = taskControl.add(&taskControl, task1, NULL,  NULL, TASK_STK_SIZE, 3);

	taskFocus = taskControl.add(&taskControl, task2, code,  NULL, TASK_STK_SIZE, 2);

	
	int i = 0;

	int counter = 0;
	putstring(FONT_W * i ++, FONT_H * 9, COL_FFFFFF, 1, "in task init");
	for (;;) {
		//io_cli();

		INTERRUPT(1);
		itoa(buffer, taskCur->runtime, 10);
		boxfill(COL_FFFFFF, FONT_W * 30, FONT_H * 9, FONT_W * 40, FONT_H * 10);
		putstring(FONT_W * 30, FONT_H * 9, COL_000000, 1, buffer);

		if (taskCur->key.linkcount > 0) {
			c = taskCur->key.pop(&taskCur->key);
			//io_sti();
			if (c) {
				putfont(FONT_W * i ++, FONT_H * 9, COL_000000, 1, sys_font + c * FONT_H);
			}
		} else {
			//io_stihlt();
		}
		INTERRUPT(0);

		OSTimeDelay(200);
	}
}

void main() {
	INT i;

	Initialize();
	
	// Reserved memory for GUI
	memman_init(&memman);
	memman_free(&memman, 0x01000000, 0x0f000000);

	// Give an available memory address & limit (reserved memory for task) 
	// and init task control
	OSInit((char *)0x10000000, 0x01000000);

	// create task and set to current task to run
	// If no task was created, then kernel will stay in main
	taskCur = taskControl.add(&taskControl, init, NULL,  NULL, TASK_STK_SIZE, 1);
	
	// Start task control
	// make sure current task has been set
	OSStartHighReady();
	
	BYTE c;
	i = 0;
	putstring(FONT_W * i ++, FONT_H * 8, COL_FFFFFF, 1, "Init main");
	int counter = 0;
	OSTask * task, * _task;
	while(1) {
		//io_cli();

		INTERRUPT(1);
		itoa(buffer, taskCur->runtime, 10);
		boxfill(COL_FFFFFF, FONT_W * 30, FONT_H * 8, FONT_W * 40, FONT_H * 9);
		putstring(FONT_W * 30, FONT_H * 8, COL_000000, 1, buffer);
		
		if (taskCur->key.linkcount > 0) {
			c = taskCur->key.pop(&taskCur->key);
			//io_sti();
			if (c) {
				putfont(FONT_W * i ++, FONT_H * 8, COL_000000, 1, sys_font + c * FONT_H);
				if (c == 'b') {
					// Activate all tasks
					taskLink.activate(&taskLink, NULL);
				}
			}
		} else {
			//io_stihlt();
		}
		INTERRUPT(0);


		OSTimeDelay(100);
	}
}