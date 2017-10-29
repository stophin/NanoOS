// MemPool.h
//

#pragma once

#include "MultiLink.h"
#include "MemStat.h"

typedef struct MemPool MemPool;
struct MemPool {
   __SUPER(ElementPool, MemPool, MemStat);
};
MemStat * MemPool_at(MemPool * that, int index) {
   return &that->pool[index];
}
void _MemPool(MemPool * that, MemStat * pool, UMAP * map, int size) {
   _ElementPool(&that->super, (MultiLinkElement *)pool, map, size);

   that->at = MemPool_at;
}
