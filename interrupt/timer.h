// timer.h
// author: stophin
//
#include "../lib/type.h"

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

#define MAX_TIMER	10

#define TIMER_USE	1
#define TIMER_NOUSE	0

typedef struct TIMER {
	INT timeout, flags;
	BYTE data;
} TIMER;

typedef struct TIMER_CTL {
	INT counter;
	INT totalCount;
	TIMER timer[MAX_TIMER];
} TIMER_CTL;

extern TIMER_CTL timer_ctl;

void int_handler20(DWORD *esp);
void init_pit();