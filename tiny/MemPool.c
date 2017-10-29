// MemPool.c
//

#include "MemPool.h"

MemStat * MemPool_at(MemPool * that, int index) {
	return &that->pool[index];
}
void _MemPool(MemPool * that, MemStat * pool, UMAP * map, int size) {
	_ElementPool(&that->super, (MultiLinkElement *)pool, map, size);

	that->at = MemPool_at;
}
