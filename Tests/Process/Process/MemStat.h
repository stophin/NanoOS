// MemStat.h
//

#pragma once

#include "MultiLink.h"

typedef int MEM_SIZE;
typedef int MEM_ADDR;
typedef int MEM_STAT;

#define MEM_OCCUPPIED	1
#define MEM_AVAILABLE	0

struct MemStat {
	__SUPER(MultiLinkElement, MemStat, NULL);
	MemStat * _prev[2];
	MemStat * _next[2];
	MEM_STAT stat;
	MEM_SIZE size;
	MEM_SIZE block;
	MEM_ADDR addr;

	void(*set)(MemStat * that, int size);
	void (*split)(MemStat * that, MemStat * mem);
	void (*merge)(MemStat * that, MemStat * mem);
};
void MemStat_set(MemStat * that, int size) {
	that->size = size;
	// 4 k block
	int div = size / 0x1000;
	int del = size - div * 0x1000;
	that->block = del ? (div + 1) * 0x1000 : div * 0x1000;
}
void MemStat_final(MemStat * that){
	//printf("MemStat final.");
}

void MemStat_split(MemStat * that, MemStat * mem) {
	that->block -= mem->block;
	that->size -= mem->block;
	mem->addr = that->addr + that->block;
}

void MemStat_merge(MemStat * that, MemStat * mem) {
	that->block += mem->block;
	that->size += mem->block;
}

MemStat * _MemStat(MemStat * that, MEM_ADDR addr, MEM_SIZE size) {
	that->prev = that->_prev;
	that->next = that->_next;
	_MultiLinkElement(&that->super, 2);

	that->split = MemStat_split;
	that->merge = MemStat_merge;
	that->set = MemStat_set;
	that->final = MemStat_final;

	that->set(that, size);
	return that;
}
