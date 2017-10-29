// Process.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

#include "OSTiny.h"
OSTask * taskFocus;

#include "KeyFIFO.h"
KeyFIFO keyFIFO;

//#define myPrintf printf

#ifndef myPrintf
int myPrintf(OSTask * task, const char * fmt, ...)
{
	if (task != taskFocus) {
		return 0;
	}
	int nret = 0;
	if (fmt == NULL) {
		return nret;
	}
	try {
		va_list args;
		va_start(args, fmt);
		nret = vprintf(fmt, args);
		va_end(args);
		return nret;
	}
	catch (void *e) {

	}
}
#endif

void task1(void *pParam)
{
	OSTask * task = (OSTask *)pParam;
	MEM_ADDR addr, addr1;
	MemStat * mem;
	int i;
	int count = 0;
	while (1)
	{
		DISABLE_INTERRUPT();
		myPrintf(task, "===============================================================\n");
		for (i = 0; i < memMan.linkcount; i++) {
			mem = memMan.get(&memMan, i);
			myPrintf(task, "d: %10X, s: %10u, b: %10u, s: %d, v: %s\n", mem->addr, mem->size, mem->block, mem->stat, (char *)mem->addr);
		}
		myPrintf(task, "===============================================================\n");

		addr = memMan.alloc(&memMan, 1000);
		ENABLE_INTERRUPT();

		OSTimeDelay(200);

		DISABLE_INTERRUPT();
		if (addr) {
			memcpy((char *)addr, "alloc", 10);
		}
		myPrintf(task, "\n");
		myPrintf(task, "===============================================================\n");
		for (i = 0; i < memMan.linkcount; i++) {
			mem = memMan.get(&memMan, i);
			myPrintf(task, "d: %10X, s: %10u, b: %10u, s: %d, v: %s\n", mem->addr, mem->size, mem->block, mem->stat, (char *)mem->addr);
		}
		myPrintf(task, "===============================================================\n");

		addr1 = memMan.realloc(&memMan, addr, 10000);
		ENABLE_INTERRUPT();

		OSTimeDelay(200);

		DISABLE_INTERRUPT();
		if (addr1) {
			memcpy((char *)addr1, "REalloc", 10);
		}
		myPrintf(task, "\n");
		myPrintf(task, "===============================================================\n");
		for (i = 0; i < memMan.linkcount; i++) {
			mem = memMan.get(&memMan, i);
			myPrintf(task, "d: %10X, s: %10u, b: %10u, s: %d, v: %s\n", mem->addr, mem->size, mem->block, mem->stat, (char *)mem->addr);
		}
		myPrintf(task, "===============================================================\n");

		memMan.free(&memMan, addr);
		memMan.free(&memMan, addr1);
		ENABLE_INTERRUPT();

		OSTimeDelay(200);

		DISABLE_INTERRUPT();
		myPrintf(task, "\n");
		myPrintf(task, "===============================================================\n");
		for (i = 0; i < memMan.linkcount; i++) {
			mem = memMan.get(&memMan, i);
			myPrintf(task, "d: %10X, s: %10u, b: %10u, s: %d, v: %s\n", mem->addr, mem->size, mem->block, mem->stat, (char *)mem->addr);
		}
		myPrintf(task, "===============================================================\n");

		ENABLE_INTERRUPT();
		DISABLE_INTERRUPT();
		myPrintf(task, "A(%d)\n", count++);
		ENABLE_INTERRUPT();

		OSTimeDelay(300);
	}
}
void task2(void *pParam)
{
	OSTask * task = (OSTask *)pParam;
	MEM_ADDR addr;
	MemStat * mem;
	int i;
	int count = 0;
	while (1) {
		DISABLE_INTERRUPT();
		addr = memMan.alloc(&memMan, 102400);
		myPrintf(task, "\n");
		myPrintf(task, "===============================================================\n");
		for (i = 0; i < memMan.linkcount; i++) {
			mem = memMan.get(&memMan, i);
			myPrintf(task, "d: %10X, s: %10u, b: %10u, s: %d, v: %s\n", mem->addr, mem->size, mem->block, mem->stat, (char *)mem->addr);
		}
		myPrintf(task, "===============================================================\n");
		ENABLE_INTERRUPT();

		DISABLE_INTERRUPT();
		myPrintf(task, "B+(%d)\n", count++);
		ENABLE_INTERRUPT();

		OSTimeDelay(200);

		DISABLE_INTERRUPT();
		memMan.free(&memMan, addr);
		myPrintf(task, "\n");
		myPrintf(task, "===============================================================\n");
		for (i = 0; i < memMan.linkcount; i++) {
			mem = memMan.get(&memMan, i);
			myPrintf(task, "d: %10X, s: %10u, b: %10u, s: %d, v: %s\n", mem->addr, mem->size, mem->block, mem->stat, (char *)mem->addr);
		}
		myPrintf(task, "===============================================================\n");
		ENABLE_INTERRUPT();
		DISABLE_INTERRUPT();
		myPrintf(task, "B-(%d)\n", count++);
		ENABLE_INTERRUPT();

		OSTimeDelay(100);
	}
}
void task3(void *pParam)
{
	OSTask * task = (OSTask *)pParam;
	int count = 0;
	while (1)
	{
		DISABLE_INTERRUPT();
		myPrintf(task, "C(%d)", count++);
		ENABLE_INTERRUPT();

		OSTimeDelay(100);
	}
}
void init(void *pParam)
{
	OSTask * task = (OSTask *)pParam;
	MemStat * mem;
	_KeyFIFO(&keyFIFO, 0);
	taskFocus = task;
	int count = 0;
	int i;
	char c;
	while (1)
	{
		DISABLE_INTERRUPT();
		taskControl.add(&taskControl, task3, NULL, (USTACK *)(memMan.alloc(&memMan, TASK_STK_SIZE * sizeof(USTACK))), 3);
		taskControl.add(&taskControl, task1, NULL, (USTACK *)(memMan.alloc(&memMan, TASK_STK_SIZE * sizeof(USTACK))), 1);
		taskControl.add(&taskControl, task2, NULL, (USTACK *)(memMan.alloc(&memMan, TASK_STK_SIZE * sizeof(USTACK))), 2);
		ENABLE_INTERRUPT();

		while (1) {


			while (keyFIFO.linkcount > 0) {
				DISABLE_INTERRUPT();
				c = keyFIFO.pop(&keyFIFO);
				myPrintf(task, "%c", c);
				switch (c) {
				case 9:	//TAB
						printf("[%d->", taskFocus->prority);
						taskFocus = taskControl.next(&taskControl, taskFocus);
						printf("%d]", taskFocus->prority);
						break;
				case 13:
						DISABLE_INTERRUPT();
						myPrintf(task, "\n");
						myPrintf(task, "===============================================================\n");
						for (i = 0; i < memMan.linkcount; i++) {
							mem = memMan.get(&memMan, i);
							myPrintf(task, "d: %10X, s: %10u, b: %10u, s: %d, v: %s\n", mem->addr, mem->size, mem->block, mem->stat, (char *)mem->addr);
						}
						myPrintf(task, "===============================================================\n");
						ENABLE_INTERRUPT();
						break;
				}
				ENABLE_INTERRUPT();
			}

			DISABLE_INTERRUPT();
			//myPrintf(task, "I(%d)", count++);
			ENABLE_INTERRUPT();
			OSTimeDelay(100);
		}
	}
}
DWORD WINAPI ThreadProc(void * pParam) {
	char c;
	while (1) {
		if (_kbhit()) {
			c = _getch();

			keyFIFO.put(&keyFIFO, c);
		}
		Sleep(10);
	}
}
#include "MultiLink.h"
int _tmain(int argc, _TCHAR* argv[])
{
	int i;
	////////////////////////////////////////
	TaskControl tasks;
	_TaskControl(&tasks);
	Task * ptask1;
	for (i = 0; i < 10; i++) {
		ptask1 = tasks.taskPool.get(&tasks.taskPool);
		_Task(ptask1, i);
		tasks.insertLink(&tasks, ptask1, NULL, NULL);
	}

	ptask1 = tasks.link;
	do {
		printf("%d->", ptask1->data);
		ptask1 = tasks.next(&tasks, ptask1);
	} while (ptask1 && ptask1 != tasks.link);
	printf("\n");
	////////////////////////////////////////

	MemStat * mem;

	OSInit();

	printf("===============================================================\n");
	for (i = 0; i < memMan.linkcount; i++) {
		mem = memMan.get(&memMan, i);
		printf("addr: %10X, size: %10u, block: %10u, status: %d\n", mem->addr, mem->size, mem->block, mem->stat);
	}
	printf("===============================================================\n");

	taskControl.add(&taskControl, init, NULL, (USTACK *)(memMan.alloc(&memMan, TASK_STK_SIZE * sizeof(USTACK))), 0);

	printf("===============================================================\n");
	for (i = 0; i < memMan.linkcount; i++) {
		mem = memMan.get(&memMan, i);
		printf("d: %10X, s: %10u, b: %10u, s: %d, v: %s\n", mem->addr, mem->size, mem->block, mem->stat, (char *)mem->addr);
	}
	printf("===============================================================\n");


	VCInit();
	// Simulating key interrupt
	HANDLE handle1 = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
	// Simulating OS tick interrupt
	timeSetEvent(1000 / OS_TICKS_PER_SEC, 0, OSTickISR, 0, TIME_PERIODIC);
	OSStartHighReady();

	_getch();
	return 0;
}
