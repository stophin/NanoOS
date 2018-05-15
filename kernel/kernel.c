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
LAYER * layer_back, * layer_mouse, * layer_win;
BYTE * buf_back, * buf_mouse, * buf_win;

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

void taskEXE(void *pParam, void *lParam) {
    int i = 0;
	char c;

	LAYER * layer_exe = (LAYER*)lParam;

    //call parameter as code
    itoa(buffer, ((int (*)())pParam)(), 10);
    layer_string(layer_ctl, layer_exe, FONT_W, FONT_H, COL_848484, COL_FFFFFF, 1, buffer);

    itoa(buffer, layer_exe->height, 10);
    layer_string(layer_ctl, layer_exe, FONT_W, FONT_H * 2, COL_848484, COL_FFFFFF, 1, buffer);

    memMan.free(&memMan, (MEM_ADDR)pParam);

    memman_free_4k(&memman, (DWORD)layer_exe->buf, layer_exe->bxsize * layer_exe->bysize);
    layer_free(layer_ctl, layer_exe);

	for (;;) {
        OSTimeDelay(100);
	}
}


void taskGUI(void *pParam, void *lParam) {
	char c;

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
	layer_set(layer_mouse, buf_mouse, 11, 17, COL_000000);
	layer_set(layer_win, buf_win, 160, 80, 0);

	draw_back(layer_back);
	draw_window(layer_win);
	draw_cursor_b(buf_mouse, layer_mouse->bxsize, 0 , 0, COL_840000, COL_FFFFFF, COL_000000);

	// set depth height
	layer_height(layer_ctl, layer_back, 0);
	layer_height(layer_ctl, layer_mouse, 100);
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
	for (;;) {
		itoa_b(timer_ctl.totalCount, buffer, 10);
		layer_string(layer_ctl, layer_win, 10, 2 * FONT_H, COL_848484, COL_FFFFFF, 2, buffer);

		OSTimeDelay(100);
	}
}



void init(void * pParam,void * lParam) {
	char c;
    int i = 0;

	taskControl.add(&taskControl, taskGUI, NULL,  NULL, TASK_STK_SIZE, 2);

    OSTask * task = NULL;
    int count = 0;
	for (;;) {
        if (taskCur->MOUSE > 0) {
            taskCur->MOUSE --;

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
            if (c) {
                putfont(FONT_W * i ++, FONT_H * 9, COL_000000, 1, sys_font + c * FONT_H);

                if (c == 'a') {
                    // Activate all tasks
                    taskLink.activate(&taskLink, NULL);
                }
                if (c == 'c' || c == 'b') {
                    BYTE * code = (BYTE *)memMan.alloc(&memMan, 1000);
                    // int code() { return 0xa;}
                    memforce(code, "\x55\x89\xE5\xB8\x0A\x00\x00\x00\x5D\xC3\x00", 11);

                    // create layer
                    LAYER * layer_exe = layer_alloc(layer_ctl);
                    BYTE * buf_exe = (BYTE *) memman_alloc_4k(&memman, 100 * 100);
                    // set layer
                    layer_set(layer_exe, buf_exe, 100, 100, 0);
                    // draw window
                    draw_window(layer_exe);
                    // set depth height
                    layer_height(layer_ctl, layer_exe, count + 3);
                    // set position
                    layer_slide(layer_ctl, layer_exe, count * 2, count * 2);

                    task = taskControl.create(&taskControl, taskEXE, (void *)code, (void *)layer_exe, TASK_STK_SIZE, 2);
                    count ++;
                    if (task) {
                        if (c == 'b') {
                            // Deactivate task immediately
                            taskLink.deactivate(&taskLink, task);
                        } else {
                            // Activate task
                            taskLink.activate(&taskLink, task);
                        }
                    }
                }
            }
        }

        OSTimeDelay(200);
	}
}

void main() {
	INT i;
    BYTE c;

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
    taskFocus = taskCur;

    // Start task control
	// make sure current task has been set
	OSStartHighReady();

	int count = 0;
	while(1) {
        itoa(buffer, count++, 10);
        boxfill(COL_FFFFFF, FONT_W * 10, FONT_H * 10, FONT_W * 20, FONT_H * 11);
        putstring(FONT_W * 10, FONT_H * 10, COL_000000, 1, buffer);
        taskControl.remove(&taskControl, taskCur);
        OSTimeDelay(100);
        // Task can be removed and recovered by other task code
        // and switch to other task
        // but can never return!!!
	}
}