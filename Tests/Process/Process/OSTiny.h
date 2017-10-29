// OSTiny.h
//

#pragma once

#include <Windows.h>

#include "MemMan.h"

typedef unsigned int USTACK;
typedef unsigned long UREG;

#define DISABLE_INTERRUPT() IntEnable=0
#define ENABLE_INTERRUPT() IntEnable=1
int IntEnable = 1;

#define ENTER_CRITICAL() OSCritical=1;DISABLE_INTERRUPT();
#define LEAVE_CRITICAL() OSCritical=0;ENABLE_INTERRUPT();
int OSCritical = 0;

int threadCounter = 0;

#define TASK_STK_SIZE 2048
#define OS_TICKS_PER_SEC 100
#define TASKS_N 10

void CALLBACK OSTickISR(unsigned int a, unsigned int b, unsigned long c, unsigned long d, unsigned long e);
USTACK *OSTaskStackInit(void(*task)(void *pd), void *pdata, USTACK *ptos);

char * memory;
MemMan memMan;

typedef struct OSTask OSTask;
struct OSTask {
	__SUPER(MultiLinkElement, OSTask, NULL);
	OSTask * _prev[2];
	OSTask * _next[2];

	USTACK *OSStack;
	USTACK *OSStackTop;
	int peroid;
	int prority;
};
void OSTask_final(OSTask * that) {
	//printf("OSTask final");
}
OSTask * _OSTask(OSTask * that, int prority) {
	that->prev = that->_prev;
	that->next = that->_next;
	_MultiLinkElement(&that->super, 2);

	that->final = OSTask_final;

	that->prority = prority;
	that->OSStack = NULL;
	that->OSStackTop = NULL;
	that->peroid = 0;

	return that;
}
////////////////////////////////////////////////////////
OSTask * taskCur;

OSTask * OSPriority();
void OSContextSwitch(OSTask * task);
////////////////////////////////////////////////////////
typedef struct OSTaskPool OSTaskPool;
struct OSTaskPool {
	__SUPER(ElementPool, OSTaskPool, OSTask);
};
OSTask * OSTaskPool_at(OSTaskPool * that, int index) {
	return &that->pool[index];
}
void _OSTaskPool(OSTaskPool * that, OSTask * pool, UMAP * map, int size) {
	_ElementPool(&that->super, (MultiLinkElement *)pool, map, size);

	that->at = OSTaskPool_at;
}
////////////////////////////////////////////////////////
typedef struct OSTaskLink OSTaskLink;
struct OSTaskLink {
	__SUPER(MultiLinkBase, OSTaskLink, OSTask);
};
void _OSTaskLink(OSTaskLink * that, int index) {
	_MultiLinkBase(&that->super, index);
}
////////////////////////////////////////////////////////
typedef struct OSTaskControl OSTaskControl;
struct OSTaskControl {
	__SUPER(MultiLinkBase, OSTaskControl, OSTask);

	OSTask pool[POOL_MAX];
	UMAP map[MAP_MAX];
	OSTaskPool taskPool;

	void(*add)(OSTaskControl * that, void(*task)(void *), void *pdata, USTACK *ptos, int prority);
	OSTask * (*remove)(OSTaskControl * that, OSTask * link);
};
void OSTaskControl_add(OSTaskControl * that, void(*task)(void *), void *pdata, USTACK *ptos, int prority) {
	if (ptos == NULL) {
		printf("Task not has a valid stack.");
		return;
	}
	OSTask * ptask;
	ptask = that->taskPool.get(&that->taskPool);
	if (ptask == NULL) {
		printf("No task avaliable");
		return;
	}
	_OSTask(ptask, prority);
	if (pdata == NULL) {
		pdata = ptask;
	}
	sprintf_s((char *)ptos, 10, "PROC:%d", prority);
	ptask->OSStack = OSTaskStackInit(task, pdata, &ptos[TASK_STK_SIZE - 1]);
	ptask->OSStackTop = ptos;
	that->insertLink(that, ptask, NULL, NULL);

	if (taskCur == NULL) {
		taskCur = ptask;
	}
}
OSTask * OSTaskControl_remove(OSTaskControl * that, OSTask * link) {
	if (link == NULL) {
		return NULL;
	}
	if (link->prority > 0) {
		link->prority = -link->prority;
	}
	else {
		that->removeLink(that, link);
		if (link->free(link) == NULL) {
			memMan.free(&memMan, (MEM_ADDR)link->OSStackTop);
			that->taskPool.back(&that->taskPool, link);
		}
	}
	return link;
}
OSTaskControl * _OSTaskControl(OSTaskControl * that, int index) {
	_MultiLinkBase(&that->super, index);
	int i;
	for (i = 0; i < POOL_MAX; i++) {
		_OSTask(&that->pool[i], 0);
	}
	_OSTaskPool(&that->taskPool, that->pool, that->map, POOL_MAX);

	that->add = OSTaskControl_add;
	that->remove = OSTaskControl_remove;

	return that;
}
OSTaskControl taskControl;

///////////////////////////////////////////////////////////////////
// Windows related
typedef struct OSContext OSContext;
struct OSContext {
	union {
		WOW64_CONTEXT context;
		struct {
			DWORD ContextFlags;
			DWORD   Dr0;
			DWORD   Dr1;
			DWORD   Dr2;
			DWORD   Dr3;
			DWORD   Dr6;
			DWORD   Dr7;
			FLOATING_SAVE_AREA FloatSave;
			DWORD   SegGs;
			DWORD   SegFs;
			DWORD   SegEs;
			DWORD   SegDs;
			DWORD   Edi;
			DWORD   Esi;
			DWORD   Ebx;
			DWORD   Edx;
			DWORD   Ecx;
			DWORD   Eax;
			DWORD   Ebp;
			DWORD   Eip;
			DWORD   SegCs;              // MUST BE SANITIZED
			DWORD   EFlags;             // MUST BE SANITIZED
			DWORD   Esp;
			DWORD   SegSs;
			BYTE    ExtendedRegisters[MAXIMUM_SUPPORTED_EXTENSION];
		};
	};
	HANDLE mainthread;
	void(*apply)(OSContext * that);
	void(*get)(OSContext * that);
	void(*save)(OSContext * that, USTACK * sp);
	void(*recover)(OSContext * that, USTACK * sp);
	void(*delay)(OSContext * that, UINT32 time);
	void(*suspend)(OSContext * that);
	void(*resume)(OSContext * that);
	bool OSStart;
	bool OSWow64;
};
void OSContext_apply(OSContext * that){
	Wow64SetThreadContext(that->mainthread, &that->context);//保存主线程上下文
}

void OSContext_get(OSContext * that) {
	Wow64GetThreadContext(that->mainthread, &that->context);//得到主线程上下文，为切换任务做准备
}

void OSContext_save(OSContext * that, USTACK * sp) {
	//在堆栈中保存相应寄存器。
	*--sp = that->Eip;//先保存eip
	*--sp = that->EFlags;//保存efl
	*--sp = that->Eax;
	*--sp = that->Ecx;
	*--sp = that->Edx;
	*--sp = that->Ebx;
	*--sp = that->Esp;//此时保存的esp是错误的，但OSTCBCur保存了正确的
	*--sp = that->Ebp;
	*--sp = that->Esi;
	*--sp = that->Edi;
	taskCur->OSStack = (USTACK *)sp;//保存当前esp
}
void OSContext_recover(OSContext * that, USTACK * sp) {
	//恢复所有处理器的寄存器
	that->Edi = *sp++;
	that->Esi = *sp++;
	that->Ebp = *sp++;
	that->Esp = *sp++;//此时上下文中得到的esp是不正确的
	that->Ebx = *sp++;
	that->Edx = *sp++;
	that->Ecx = *sp++;
	that->Eax = *sp++;
	that->EFlags = *sp++;
	that->Eip = *sp++;
	that->Esp = (UREG)sp;//得到正确的esp
}
void OSContext_delay(OSContext * that, UINT32 time) {
	if (OSCritical) {
		return;
	}
	ENTER_CRITICAL();
	taskCur->peroid += time;
	LEAVE_CRITICAL();
	Sleep(time * 50 / OS_TICKS_PER_SEC );
	//that->suspend(that);
}

void OSContext_suspend(OSContext * that) {
	int ret;
	do {
		//threadCounter++;
		//printf("S");
		ret = Wow64SuspendThread(that->mainthread);
	} while (ret < 0 && ret != -1);
}
void OSContext_resume(OSContext * that) {
	int ret;
	do {
		//threadCounter--;
		//printf("R");
		ret = ResumeThread(that->mainthread);
	} while (ret > 1 && ret != -1);
}
void _OSContext(OSContext * that) {
	that->context.ContextFlags = CONTEXT_CONTROL;

	that->apply = OSContext_apply;
	that->delay = OSContext_delay;
	that->get = OSContext_get;
	that->save = OSContext_save;
	that->recover = OSContext_recover;
	that->resume = OSContext_resume;
	that->suspend = OSContext_suspend;

	that->OSStart = false;
	that->OSWow64 = false;
}
OSContext Context;

void VCInit(void)
{
	HANDLE cp, ct;
	cp = GetCurrentProcess();//得到当前进程句柄
	ct = GetCurrentThread();//得到当前线程伪句柄
	DuplicateHandle(cp, ct, cp, &Context.mainthread, 0, TRUE, 2);//伪句柄转换,得到线程真句柄
	printf("%X %X %X\n", cp, ct, Context.mainthread);
	IsWow64Process(cp, (BOOL *)&Context.OSWow64);
	printf("Procss %s-bit running\n", Context.OSWow64 ? "64":"32");
}
///////////////////////////////////////////////////////////////////

void OSInit() {
	_MemMan(&memMan, 1);
	MemStat * mem;

	memory = (char *)malloc(0x1000000);
	memset(memory, 0, 0x1000000);
	mem = memMan.memPool.get(&memMan.memPool);
	mem->addr = (MEM_ADDR)memory;
	mem->set(mem, 0x1000000);
	memMan.add(&memMan, mem);

	_OSContext(&Context);
	_OSTaskControl(&taskControl, 0);
}

void OSTimeDelay(UINT32 time) {
	Context.delay(&Context, time);
}

OSTask * OSPriority() {
	OSTask * taskNext = NULL;
	OSTask * task = taskControl.next(&taskControl, taskCur), *_task = taskCur;
	int prority = 0;
	if (task == NULL) {
		task = taskControl.link;
		_task = task;
	}
	if (task == NULL) {
		return NULL;
	}
	OSTaskLink link;
	_OSTaskLink(&link, 1);
	do {
		if (task->peroid > 0) {
			task->peroid--;
			if (task->peroid <= 0) {
				task->peroid = 0;
			}
		}
		if (task->prority < 0) {
			// prority 0 can not be removed
			link.insertLink(&link, task, NULL, NULL);
		}
		else if (task != taskCur && task->peroid == 0 && prority <= task->prority) {
			taskNext = task;
			prority = task->prority;
		}
		task = taskControl.next(&taskControl, task);
	} while (task && task != _task);
	task = link.link;
	if (task) {
		do {
			_task = link.next(&link, task);

			if (task->prority < 0) {
				link.removeLink(&link, task);
				taskControl.remove(&taskControl, task);
			}

			task = _task;
		} while (link.link && task && task != link.link);
	}
	if (taskNext == NULL) {
		taskNext = taskControl.next(&taskControl, taskCur);
	}
	if (taskNext == NULL) {
		taskNext = taskControl.link;
	}
	//printf("[%d]", taskNext->prority);
	return taskNext;
}

void OSContextSwitch(OSTask * task)
{
	if (taskCur == NULL) {
		return;
	}
	OSTask * taskNext;
	if (task) {
		taskNext = task;
	}
	else {
		taskNext = OSPriority();
	}
	if (taskNext == NULL) {
		return;
	}

	Context.get(&Context);//得到主线程上下文，为切换任务做准备

	Context.save(&Context, (USTACK *)Context.Esp);//得到主线程当前堆栈指针

	taskCur = taskNext;//得到当前就绪最高优先级任务的tcb

	Context.recover(&Context, taskCur->OSStack);

	Context.apply(&Context); //保存主线程上下文

}
void CALLBACK OSTickISR(unsigned int a, unsigned int b, unsigned long c, unsigned long d, unsigned long e)
{
	if (!Context.OSStart) {
		return;
	}
	if (OSCritical) {
		return;
	}
	if (!IntEnable)
		return;//如果当前中断被屏蔽则返回
	Context.suspend(&Context);//中止主线程的运行，模拟中断产生.但没有保存寄存器
	if (!IntEnable)
	{
		//在suspendthread完成以前，flagEn可能被再次改掉
		Context.resume(&Context);//模拟中断返回，主线程得以继续执行
		return;//如果当前中断被屏蔽则返回
	}

	ENTER_CRITICAL();

	Context.suspend(&Context);
	OSContextSwitch(NULL);//由于不能使用中断返回指令，所以此函数是要返回的
	Context.resume(&Context);//模拟中断返回

	LEAVE_CRITICAL();
}
USTACK *OSTaskStackInit(void(*task)(void *pd), void *pdata, USTACK *ptos)
{
	//printf("%X\n", ptos);
	UREG *stk;//console 下寄存器为32位宽
	stk = (UREG *)ptos; /* Load stack pointer */
	*--stk = (UREG)pdata; /* Simulate call to function with argument */
	*--stk = (UREG)0x00000000;//子程序是从当前esp＋4处取得传入的参数，所以此处要空出4个字节
	*--stk = (UREG)task; /* Put pointer to task on top of stack */
	*--stk = (UREG)0x00000202;/* EFL = 0X00000202*/
	*--stk = (UREG)0xAAAAAAAA; /* EAX = 0xAAAAAAAA */
	*--stk = (UREG)0xCCCCCCCC; /* ECX = 0xCCCCCCCC */
	*--stk = (UREG)0xDDDDDDDD; /* EDX = 0xDDDDDDDD */
	*--stk = (UREG)0xBBBBBBBB; /* EBX = 0xBBBBBBBB */
	*--stk = (UREG)0x00000000; /* ESP = 0x00000000 esp可以任意，因为 */
	*--stk = (UREG)0x11111111; /* EBP = 0x11111111 */
	*--stk = (UREG)0x22222222; /* ESI = 0x22222222 */
	*--stk = (UREG)0x33333333; /* EDI = 0x33333333 */

	return ((USTACK *)stk);
}
void OSStartHighReady(void)
{
	if (taskCur == NULL) {
		printf("No task.");
		return;
	}
	USTACK ** stack = &taskCur->OSStack;
	if (stack == NULL) {
		printf("No task stack.");
		return;
	}
	Context.OSStart = true;
	_asm
	{
		mov ebx, [stack];//OSTCBCur结构的第一个参数就是esp
		mov esp, [ebx];//恢复堆栈
		popad;//恢复所有通用寄存器，共8个
		popfd;//恢复标志寄存器
		ret;//ret 指令相当于pop eip 但保护模式下不容许使用eip
		;//永远都不返回
	}
}