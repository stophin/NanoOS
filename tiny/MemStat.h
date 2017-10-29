// MemStat.h
//

#ifndef _MEMSTAT_
#define _MEMSTAT_

#include "MultiLink.h"

typedef int MEM_SIZE;
typedef int MEM_ADDR;
typedef int MEM_STAT;

#define MEM_OCCUPPIED	1
#define MEM_AVAILABLE	0

///////////////////////////////////////////////////////////
typedef struct MemStat MemStat;
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
void MemStat_set(MemStat * that, int size);
void MemStat_final(MemStat * that);
void MemStat_split(MemStat * that, MemStat * mem);
void MemStat_merge(MemStat * that, MemStat * mem);
MemStat * _MemStat(MemStat * that, MEM_ADDR addr, MEM_SIZE size) ;
///////////////////////////////////////////////////////////

#endif