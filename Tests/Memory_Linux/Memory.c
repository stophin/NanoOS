
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "MemMan.h"
char * memory;
MemMan memMan;
int main() {
	_MemMan(&memMan, 1);
	memory = (char *)malloc(0x10000000);
	memset(memory, 0, 0x10000000);
	MemStat * mem = memMan.memPool.get(&memMan.memPool);
	mem->addr = (MEM_ADDR)memory;
	mem->set(mem, 0x10000000);
	memMan.add(&memMan, mem);
	
	MEM_ADDR addr = memMan.alloc(&memMan, 1000);
	if (addr) {
		memcpy((char *)addr, "alloc", 10);
	}
	int i;
	for (i = 0; i < memMan.linkcount; i ++) {
		mem = memMan.get(&memMan, i);
		printf( "d: %10X, s: %10u, b: %10u, s: %d, v: %s\n", mem->addr, mem->size, mem->block, mem->stat, (char *)mem->addr);
	}
	printf("========================\n");
	
	addr = memMan.realloc(&memMan, addr, 10000);
	if (addr) {
		memcpy((char *)addr, "RE-alloc", 10);
	}
	for (i = 0; i < memMan.linkcount; i ++) {
		mem = memMan.get(&memMan, i);
		printf( "d: %10X, s: %10u, b: %10u, s: %d, v: %s\n", mem->addr, mem->size, mem->block, mem->stat, (char *)mem->addr);
	}
	printf("========================\n");
	addr = memMan.realloc(&memMan, addr, 10);
	if (addr) {
		memcpy((char *)addr, "RE-alloc", 10);
	}
	for (i = 0; i < memMan.linkcount; i ++) {
		mem = memMan.get(&memMan, i);
		printf( "d: %10X, s: %10u, b: %10u, s: %d, v: %s\n", mem->addr, mem->size, mem->block, mem->stat, (char *)mem->addr);
	}
	printf("========================\n");
	
	memMan.free(&memMan, addr);
	
	for (i = 0; i < memMan.linkcount; i ++) {
		mem = memMan.get(&memMan, i);
		printf( "d: %10X, s: %10u, b: %10u, s: %d, v: %s\n", mem->addr, mem->size, mem->block, mem->stat, (char *)mem->addr);
	}
	printf("========================\n");
	
	for (i = 0; i < POOL_MAX / 2; i++) {
		addr = memMan.alloc(&memMan, 4096);
		if (!addr) {
			printf("alloc error: @%d\n", i);
		} else {
			memcpy((char *)addr, "alloc", 10);
		}
	}
	
	for (i = 0; i < memMan.linkcount; i ++) {
		mem = memMan.get(&memMan, i);
		printf( "d: %10X, s: %10u, b: %10u, s: %d, v: %s\n", mem->addr, mem->size, mem->block, mem->stat, (char *)mem->addr);
	}
	printf("========================\n");
	
	
	for (i = 0; i < memMan.linkcount; i ++) {
		mem = memMan.get(&memMan, i);
		if (mem->stat != 1) {
			continue;
		}
		addr = memMan.realloc(&memMan, mem->addr, 4098);
		if (!addr) {
			printf("RE-alloc error: @%d\n", i);
		} else {
			memcpy((char *)addr, "RE-alloc", 10);
		}
	}
	
	for (i = 0; i < memMan.linkcount; i ++) {
		mem = memMan.get(&memMan, i);
		printf( "d: %10X, s: %10u, b: %10u, s: %d, v: %s\n", mem->addr, mem->size, mem->block, mem->stat, (char *)mem->addr);
	}
	printf("========================\n");
	
	free(memory);
}
