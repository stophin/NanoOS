// OSTiny.h
//

#include "../interrupt/interrupt.h"

#include "OSTiny.h"

#include "../drivers/screen.h"

/////////////////////////////////////////////////////////////
MemMan memMan;
/////////////////////////////////////////////////////////////
int IntDisabled = 0;
int OSCritical = 0;
/////////////////////////////////////////////////////////////

void OSTask_final(OSTask * that) {
}
void OSTask_set(OSTask * that, DWORD esp, DWORD task_ptr, int offset) {
	that->tss.eip = task_ptr;
	that->tss.eflags = 0x00000202;
	that->tss.eax = 0;
	that->tss.ecx = 0;
	that->tss.edx = 0;
	that->tss.ebx = 0;
	that->tss.esp = esp;
	that->tss.ebp = 0;
	that->tss.esi = 0;
	that->tss.edi = 0;
	that->tss.es = 2 * 8;
	that->tss.cs = 1 * 8;
	that->tss.ss = 2 * 8;
	that->tss.ds = 2 * 8;
	that->tss.fs = 2 * 8;
	that->tss.gs = 2 * 8;
	
	// offset is set in pool and cannot be changed
	//that->offset = offset;
	
	SEGM_DESCRIPTOR * gdt = (SEGM_DESCRIPTOR *) ADR_GDT;
	// segment limit is 103 bytes, and do not need to load lgdt
	set_segmdesc(gdt + that->offset, 103, (DWORD) &that->tss, 0x0089);//89 386 task state segment(TSS)
}
OSTask * _OSTask(OSTask * that, int prority) {
	that->prev = that->_prev;
	that->next = that->_next;
	_MultiLinkElement(&that->super, TASK_LINK);

	_KeyFIFO(&that->key, 0);
	that->MOUSE = 0;

	that->final = OSTask_final;
	that->set = OSTask_set;

	that->prority = prority;
	that->OSStack = NULL;
	that->OSStackTop = NULL;
	that->peroid = 0;
	that->runtime = 0;
	
	that->tss.ldtr = 0;
	that->tss.iomap = 0x40000000;

	return that;
}
/////////////////////////////////////////////////////////////
OSTask * taskCur = NULL, * taskFocus = NULL;
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
OSTask * OSTaskPool_at(OSTaskPool * that, int index) {
	return &that->pool[index];
}
void _OSTaskPool(OSTaskPool * that, OSTask * pool, UMAP * map, int size) {
	_ElementPool(&that->super, (MultiLinkElement *)pool, map, size);

	that->at = OSTaskPool_at;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void _OSTaskLink(OSTaskLink * that, int index) {
	_MultiLinkBase(&that->super, index);
	
	that->activate = OSTaskLink_activate;
    that->deactivate = OSTaskLink_deactivate;
}
OSTask * OSTaskLink_activate(OSTaskLink * that, OSTask * link) {
    if (link) {
        taskControl.insertLink(&taskControl, link, NULL, NULL);
    } else {
        OSTask * task, * _task;
        task = that->next(that, that->link);
        if (task) {
            do {
                _task = that->next(that, task);

                that->removeLink(that, task);
                taskControl.insertLink(&taskControl, task, NULL, NULL);

                task = _task;
            } while(that->link && task);
        }
    }
    return link;
}
OSTask * OSTaskLink_deactivate(OSTaskLink * that, OSTask * link) {
    if (link) {
        that->insertLink(that, link, NULL, NULL);
    }
    return link;
}
/////////////////////////////////////////////////////////////
OSTask * OSTaskControl_create(OSTaskControl * that, void TASK_FN, void *pdata, void *ldata, int size, int prority) {
	OSTask * ptask;
	USTACK * ptos = (USTACK *)(memMan.alloc(&memMan, size * sizeof(USTACK)));
	if (ptos == NULL) {
		return NULL;
	}
	ptask = that->taskPool.get(&that->taskPool);
	if (ptask == NULL) {
		return NULL;
	}
	_OSTask(ptask, prority);
	if (pdata == NULL) {
		pdata = ptask;
	}
	ptask->OSStack = OSTaskStackInit(task, pdata, ldata, &ptos[size - 1]);
	ptask->OSStackTop = ptos;
	// ldt start from 3 (1 code seg, 2 data seg)
	// the third parameter offset is no longer in use
	ptask->set(ptask, (DWORD)ptask->OSStack, (DWORD)task, that->linkcount + 2);
	
	return ptask;
}
OSTask * OSTaskControl_add(OSTaskControl * that, void TASK_FN, void *pdata, void *ldata, int size, int prority) {
	return that->insertLink(that, that->create(that, task, pdata, ldata, size, prority) , NULL, NULL);
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
	// force task switch
	OSTickISR();
	return link;
}
OSTaskControl * _OSTaskControl(OSTaskControl * that, int index) {
	int i;
	_MultiLinkBase(&that->super, index);
	for (i = 0; i < TASK_MAX; i++) {
		_OSTask(&that->pool[i], 0);
		// once offset is set it can not be changed
		that->pool[i].offset = 3 + i;
	}
	_OSTaskPool(&that->taskPool, that->pool, that->map, TASK_MAX);

	that->add = OSTaskControl_add;
	that->remove = OSTaskControl_remove;
	that->create = OSTaskControl_create;

	return that;
}
/////////////////////////////////////////////////////////////
OSTaskControl taskControl;
OSTaskLink taskLink, _link;
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void OSContext_suspend(OSContext * that) {
}

void OSContext_get(OSContext * that) {
}

void OSContext_save(OSContext * that, USTACK * sp) {
}

void OSContext_recover(OSContext * that, USTACK * sp) {
	if (taskFocus == NULL) {
		taskFocus = taskCur;
	}
}

void OSContext_apply(OSContext * that){
}

void OSContext_resume(OSContext * that) {
	far_jump(0, taskCur->offset * 8);
}

void Sleep(UINT32 time) {
	io_stihlt();
}

void OSContext_delay(OSContext * that, UINT32 time) {
	ENTER_CRITICAL();
	taskCur->peroid = time / OS_TICKS_PER_SEC;
	LEAVE_CRITICAL();
	Sleep(0);
}

void _OSContext(OSContext * that) {

	that->apply = OSContext_apply;
	that->delay = OSContext_delay;
	that->get = OSContext_get;
	that->save = OSContext_save;
	that->recover = OSContext_recover;
	that->resume = OSContext_resume;
	that->suspend = OSContext_suspend;

	that->OSStart = 0;
}
/////////////////////////////////////////////////////////////
OSContext Context;
/////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
void OSTimeDelay(UINT32 time) {
	Context.delay(&Context, time);
}

OSTask * OSPriority() {
	OSTask * taskNext = NULL;
	OSTask * task, *_task;
	OSTaskControl ctl;
	int prority = 0;
	task = taskControl.next(&taskControl, taskCur);
	_task = taskCur;

	if (task == NULL) {
		task = taskControl.link;
		_task = task;
	}
	
	do {
		if (task->peroid > 0) {
			task->peroid--;
			if (task->peroid <= 0) {
				task->peroid = 0;
			}
		}
		if (task->prority < 0) {
			// prority 0 can not be removed
			_link.insertLink(&_link, task, NULL, NULL);
		}
		else if (taskNext == NULL && task != taskCur && task->peroid == 0 && prority <= task->prority) {
			taskNext = task;
			prority = task->prority;
		}
		task = taskControl.next(&taskControl, task);
	} while (task && task != _task);
	task = _link.next(&_link, _link.link);
	if (task) {
		do {
			_task = _link.next(&_link, task);

			if (task->prority < 0) {
				_link.removeLink(&_link, task);
				taskControl.remove(&taskControl, task);
			}

			task = _task;
		} while (_link.link && task);
	}
	if (taskNext == NULL) {
		taskNext = taskControl.next(&taskControl, taskCur);
	}
	if (taskNext == NULL) {
		taskNext = taskControl.link;
	}
	taskNext->runtime ++;
	return taskNext;
}

void OSContextSwitch(OSTask * task)
{
	OSTask * taskNext;
	if (taskCur == NULL) {
		return;
	}
	if (task) {
		taskNext = task;
	}
	else {
		taskNext = OSPriority();
	}
	if (taskNext == NULL) {
		return;
	}

	Context.get(&Context);// Get main thread context, prepare for context switch

	Context.save(&Context, (USTACK *)Context.Esp);// Get main thread current stack pointer

	taskCur = taskNext;// Get next task according to prority

	Context.recover(&Context, taskCur->OSStack);

	Context.apply(&Context); // Save and swicth context

}
void OSTickISR()
{
	if (!Context.OSStart) {
		return;
	}
	if (OSCritical) {
		return;
	}
	if (IntDisabled) {
		return;// If interrupt is disabled
	}
	
	ENTER_CRITICAL();

	OSContextSwitch(NULL);

	LEAVE_CRITICAL();
	
	Context.resume(&Context);// Simulate interrupt return
}
USTACK *OSTaskStackInit(void TASK_FN, void *pdata, void *ldata, USTACK *ptos)
{
	UREG *stk;//console 32-bit
	stk = (UREG *)ptos; /* Load stack pointer */
	*--stk = (UREG)task; /* Put pointer to task on top of stack */
	//set values for registers in stack
	//a tack will never return 
	//so set them as you wish or remove them both will be OK
	*--stk = (UREG)0x00000202;/* EFL = 0X00000202*/
	*--stk = (UREG)0xAAAAAAAA; /* EAX = 0xAAAAAAAA */
	*--stk = (UREG)0xBBBBBBBB; /* EBX = 0xBBBBBBBB */
	*--stk = (UREG)0xCCCCCCCC; /* ECX = 0xCCCCCCCC */
	*--stk = (UREG)0xDDDDDDDD; /* EDX = 0xDDDDDDDD */
	*--stk = (UREG)0x00000000; /* ESP = 0x00000000  esp can be any*/
	*--stk = (UREG)0x11111111; /* EBP = 0x11111111 */
	*--stk = (UREG)0x22222222; /* ESI = 0x22222222 */
	*--stk = (UREG)0x33333333; /* EDI = 0x33333333 */
	//these parameters are important
	*--stk = (UREG)ldata; /* Simulate call to function with argument */
	*--stk = (UREG)pdata; /* Simulate call to function with argument */
	*--stk = (UREG)0x00000000; //Paramter is esp + 4
	return ((USTACK *)stk);
}
void OSInit(char * memory, int limit) {
	MemStat * mem;
	int i;
	_MemMan(&memMan, 1);

	mem = memMan.memPool.get(&memMan.memPool);
	if (mem == NULL) {
		return;
	}
	mem->addr = (MEM_ADDR)memory;
	mem->set(mem, limit);
	memMan.add(&memMan, mem);

	_OSContext(&Context);
	_OSTaskControl(&taskControl, 0);
	_OSTaskLink(&_link, 1);
	_OSTaskLink(&taskLink, 2);

	// create the first task with priority 0
	// this task is the task which invokes OSInit function
	// and cannot be removed from task queue
	taskFocus = taskControl.add(&taskControl, NULL, NULL, NULL, TASK_STK_SIZE, 0);
	// load the task to ldtr
	load_ldtr(taskFocus->offset * 8);
}

void OSStartHighReady(void)
{
	USTACK ** stack;
	if (taskCur == NULL) {
		return;
	}

	stack = &taskCur->OSStack;
	if (stack == NULL) {
		return;
	}
	Context.OSStart = 1;
	
	far_jump(0, taskCur->offset * 8);
}