// MemMan.h
//

#ifndef _MEMMAN_
#define _MEMMAN_

#include "MultiLink.h"
#include "MemStat.h"
#include "MemPool.h"


///////////////////////////////////////////////////////////
#define MEM_MAX 30
#define MEM_MAP GET_MAP_SIZE(MEM_MAX)
typedef struct MemMan MemMan;
struct MemMan {
	__SUPER(MultiLinkBase, MemMan, MemStat);

	MemStat pool[MEM_MAX];
	UMAP map[MEM_MAP];
	MemPool memPool;
	void(*add)(MemMan * that, MemStat * link);
	MemStat * (*getAddr)(MemMan * that, MEM_ADDR addr, int stat);
	MEM_ADDR(*realloc)(MemMan * that, MEM_ADDR addr, MEM_SIZE size);
	MEM_ADDR(*alloc)(MemMan * that, MEM_SIZE size);
	void (*free)(MemMan * that, MEM_ADDR addr);
	void (*merge)(MemMan * that, MemStat * start, MemStat * end);
	void (*split)(MemMan * that, MemStat * tango, MemStat * item);
	MemStat * (*remove)(MemMan * that, MemStat * link);
};
void MemMan_add(MemMan * that, MemStat * link);
MemStat * MemMan_getAddr(MemMan * that, MEM_ADDR addr, int stat);
MEM_ADDR MemMan_realloc(MemMan * that, MEM_ADDR addr, MEM_SIZE size);
MEM_ADDR MemMan_alloc(MemMan * that, MEM_SIZE size);

void MemMan_free(MemMan * that, MEM_ADDR addr);
void MemMan_merge(MemMan * that, MemStat * start, MemStat * end);
MemStat * MemMan_remove(MemMan * that, MemStat * link);
void MemMan_split(MemMan * that, MemStat * tango, MemStat * item);
MemMan * _MemMan(MemMan * that, int index);
///////////////////////////////////////////////////////////

#endif