// MemStat.c
//


#include "MemStat.h"


///////////////////////////////////////////////////////////
void MemStat_set(MemStat * that, int size) {
	// 4 k block
	int div = size / 0x1000;
	int del = size - div * 0x1000;
	that->block = del ? (div + 1) * 0x1000 : div * 0x1000;
	that->size = size;
}
void MemStat_final(MemStat * that){
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
///////////////////////////////////////////////////////////