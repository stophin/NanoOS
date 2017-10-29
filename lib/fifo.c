// fifo.c
// author: stophin
//
#include "../lib/fifo.h"

// initialize FIFO stack, using self defined buffer and size
void fifo_init(FIFO * fifo, INT size, BYTE * buff) {
	fifo->size = size;
	fifo->buff = buff;
	fifo->free = size;
	fifo->flag = 0;
	fifo->p = 0;	// the next data to write
	fifo->q = 0;	// the next data to read

	return;
}

// put data into FIFO stack
INT fifo_put(FIFO * fifo, BYTE data) {
	if (fifo->free == 0) {
		// no free space
		fifo->flag |= FLAG_OVERFLOW;
		return -1;
	}

	fifo->buff[fifo->p] = data;
	fifo->p ++;
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->free --;
	return 0;
}

// get data from FIFO stack
INT fifo_get(FIFO * fifo) {
	BYTE data;
	if (fifo->free == fifo->size) {
		// if stack is empty
		return -1;
	}
	data = fifo->buff[fifo->q];
	fifo->q ++;
	if (fifo->q == fifo->size) {
			fifo->q = 0;
	}
	fifo->free ++;
	return data;
}

// get data count
INT fifo_stat(FIFO * fifo) {
	return fifo->size - fifo->free;
}