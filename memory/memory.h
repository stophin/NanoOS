#include "../lib/type.h"

DWORD mem_test(DWORD start, DWORD end);
DWORD mem_test_sub(DWORD start, DWORD end);

#define MEMMAN_BLOCKS	4090	// 4090 blocks 

#ifndef _MEMORY_
#define _MEMORY_

typedef struct BLOCKINFO {
	DWORD addr, size;
} BLOCKINFO;

typedef struct MEMMAN {
	DWORD blocks, maxblocks, lostsize, losts;
	BLOCKINFO block[MEMMAN_BLOCKS];	// blocks
} MEMMAN;

#endif

extern MEMMAN memman;

void memman_init(MEMMAN * man);
DWORD memman_total(MEMMAN * man);
DWORD memman_alloc(MEMMAN * man, DWORD size);
DWORD memman_free(MEMMAN * man, DWORD addr, DWORD size);
DWORD memman_alloc_4k(MEMMAN * man, DWORD size);
DWORD memman_free_4k(MEMMAN * man, DWORD addr, DWORD size);