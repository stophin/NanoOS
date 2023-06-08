//int code() { return 0xa;}
#include "tiny/OSTiny.h"
//extern OSTask* taskCur;
int code(OSTask* task) {
	//return task->runtime;
	//return taskCur ? taskCur->runtime : 0;
	__asm__("int $0x30");
	return 0;
}