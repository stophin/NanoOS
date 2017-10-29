// fifo.h
// author: stophin
//
#include "../lib/type.h"

#ifndef _FIFO_
#define _FIFO_

#define FLAG_OVERFLOW	0x0001

typedef struct FIFO {
	BYTE * buff;
	INT p, q, size, free, flag;
} FIFO;

void fifo_init(FIFO * fifo, INT size, BYTE * buff);
INT fifo_put(FIFO * fifo, BYTE data);
INT fifo_get(FIFO * fifo);

#endif