// OSTiny.h
//

#ifndef _OSTiny_
#define _OSTiny_

#include "../interrupt/interrupt.h"
#include "../lib/lib.h"

#include "KeyFIFO.h"

#include "MemMan.h"
extern MemMan memMan;

////////////////////////////////////////////////////////
extern int IntDisabled;
#define DISABLE_INTERRUPT() IntDisabled=1
#define ENABLE_INTERRUPT() IntDisabled=0

extern int OSCritical;
#define ENTER_CRITICAL() OSCritical=1
#define LEAVE_CRITICAL() OSCritical=0

////////////////////////////////////////////////////////

typedef unsigned int USTACK;
typedef unsigned long UREG;

#define TASK_STK_SIZE 2048
#define OS_TICKS_PER_SEC 100
#define TASKS_N 10

////////////////////////////////////////////////////////
#define TASK_FN (*task)(void *, void *)
////////////////////////////////////////////////////////
typedef struct OSTSS OSTSS;
struct OSTSS {
	DWORD backlink;
	DWORD esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	DWORD eip, eflags, eax, ecx, edx, ebx,  esp, ebp, esi, edi;
	DWORD es, cs, ss, ds, fs, gs;
	DWORD ldtr, iomap;
};
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
#define TASK_LINK 3

typedef struct OSTask OSTask;
struct OSTask {
	__SUPER(MultiLinkElement, OSTask, NULL);
	OSTask * _prev[TASK_LINK];
	OSTask * _next[TASK_LINK];

	USTACK *OSStack;
	USTACK *OSStackTop;
	int peroid;
	int runtime;
	int prority;
	OSTSS tss;
	int offset;
	KeyFIFO key;
	int MOUSE;
	void (*set)(OSTask * that, DWORD esp, DWORD task_ptr, int offset);
};
extern OSTask * taskCur, * taskFocus;
void OSTask_final(OSTask * that);
void OSTask_set(OSTask * that, DWORD esp, DWORD task_ptr, int offset);
OSTask * _OSTask(OSTask * that, int prority);
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
typedef struct OSTaskPool OSTaskPool;
struct OSTaskPool {
	__SUPER(ElementPool, OSTaskPool, OSTask);
};
OSTask * OSTaskPool_at(OSTaskPool * that, int index);
void _OSTaskPool(OSTaskPool * that, OSTask * pool, UMAP * map, int size);
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
typedef struct OSTaskLink OSTaskLink;
struct OSTaskLink {
	__SUPER(MultiLinkBase, OSTaskLink, OSTask);

	OSTask * (*activate)(OSTaskLink * that, OSTask * link);
};
extern OSTaskLink taskLink;
void _OSTaskLink(OSTaskLink * that, int index);
OSTask * OSTaskLink_activate(OSTaskLink * that, OSTask * link);
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
#define TASK_MAX 30
#define TASK_MAP GET_MAP_SIZE(TASK_MAX)
typedef struct OSTaskControl OSTaskControl;
struct OSTaskControl {
	__SUPER(MultiLinkBase, OSTaskControl, OSTask);

	OSTask pool[TASK_MAX];
	UMAP map[TASK_MAP];
	OSTaskPool taskPool;

	OSTask * (*create)(OSTaskControl * that, void TASK_FN, void *pdata,void *ldata, int size, int prority);
	OSTask * (*add)(OSTaskControl * that, void TASK_FN, void *pdata,void *ldata, int size, int prority);
	OSTask * (*remove)(OSTaskControl * that, OSTask * link);
};
extern OSTaskControl taskControl;
OSTask * OSTaskControl_create(OSTaskControl * that, void TASK_FN, void *pdata,void *ldata, int size,  int prority);
OSTask * OSTaskControl_add(OSTaskControl * that, void TASK_FN, void *pdata,void *ldata, int size,  int prority);
OSTask * OSTaskControl_remove(OSTaskControl * that, OSTask * link);
OSTaskControl * _OSTaskControl(OSTaskControl * that, int index);
////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Simulation operation system related
typedef struct OSContext OSContext;
struct OSContext {
	union {
		OSTSS context;
		struct {
			DWORD esp0, ss0, esp1, ss1, esp2, ss2, cr3;
			DWORD Eip, EFlags, Eax, Ecx, Edx, Ebx,  Esp, Ebp, Esi, Edi;
			DWORD es, cs, ss, ds, fs, gs;
			DWORD ldtr, iomap;
		};
	};
	void(*apply)(OSContext * that);
	void(*get)(OSContext * that);
	void(*save)(OSContext * that, USTACK * sp);
	void(*recover)(OSContext * that, USTACK * sp);
	void(*delay)(OSContext * that, UINT32 time);
	void(*suspend)(OSContext * that);
	void(*resume)(OSContext * that);
	int OSStart;
};
void OSContext_apply(OSContext * that);
void OSContext_get(OSContext * that) ;
void OSContext_save(OSContext * that, USTACK * sp);
void OSContext_recover(OSContext * that, USTACK * sp);
void Sleep(UINT32 time);
void OSContext_delay(OSContext * that, UINT32 time);
void OSContext_suspend(OSContext * that);
void OSContext_resume(OSContext * that);
void _OSContext(OSContext * that);
extern OSContext Context;


///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
void OSInit(char * memory, int limit);
void OSTimeDelay(UINT32 time);
OSTask * OSPriority();
void OSContextSwitch(OSTask * task);
void OSTickISR();
USTACK *OSTaskStackInit(void TASK_FN, void *pdata,void *ldata, USTACK *ptos);
void OSStartHighReady(void);
///////////////////////////////////////////////////////////////////

#endif