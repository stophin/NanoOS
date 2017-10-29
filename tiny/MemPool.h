// MemPool.h
//

#ifndef _MEMPOOL_
#define _MEMPOOL_

#include "MultiLink.h"
#include "MemStat.h"

///////////////////////////////////////////////////////////
typedef struct MemPool MemPool;
struct MemPool {
	__SUPER(ElementPool, MemPool, MemStat);
};
MemStat * MemPool_at(MemPool * that, int index);
void _MemPool(MemPool * that, MemStat * pool, UMAP * map, int size);
///////////////////////////////////////////////////////////

#endif