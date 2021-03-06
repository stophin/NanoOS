// MemMan.h
//

#pragma once

#include "MultiLink.h"
#include "MemStat.h"
#include "MemPool.h"

typedef struct MemMan MemMan;
struct MemMan {
	__SUPER(MultiLinkBase, MemMan, MemStat);

	MemStat pool[POOL_MAX];
	UMAP map[MAP_MAX];
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
void MemMan_add(MemMan * that, MemStat * link) {
	that->insertLink(that, link, NULL, NULL);
}
MemStat * MemMan_getAddr(MemMan * that, MEM_ADDR addr, int stat = MEM_AVAILABLE) {
	if (that->link == NULL) {
		return NULL;
	}
	MemStat * mem = that->link;
	do {
		if (mem->stat == stat && mem->addr == addr) {
			return mem;
		}
		mem = that->next(that, mem);
	} while (mem && mem != that->link);
	return NULL;
}
MEM_ADDR MemMan_realloc(MemMan * that, MEM_ADDR addr, MEM_SIZE size) {
	if (that->link == NULL) {
		return NULL;
	}
	MemStat * mem = that->getAddr(that, addr, 1);
	if (mem == NULL) {
		return NULL;
	}
	//MemStat * item = new MemStat(0, size);
	MemStat * item = that->memPool.get(&that->memPool);
	if (item == NULL) {
		return NULL;
	}
	item->set(item, size);
	if (mem->block == item->block) {
		mem->set(mem, size);
		//delete item;
		that->remove(that, item);
		return addr;
	}
	MemStat * prev, *next;
	if (mem->block > item->block) {
		that->split(that, mem, item);
		mem->stat = MEM_AVAILABLE;
		item->stat = MEM_OCCUPPIED;

		prev = that->prev(that, mem);
		if (mem != that->link && prev->stat == MEM_AVAILABLE) {
			that->merge(that, prev, mem);
		}
		return item->addr;
	}
	prev = that->prev(that, mem);
	next = that->next(that, mem);

	int mark = 0;
	if (item != that->link && prev->stat == MEM_AVAILABLE) {
		if (mem->block + prev->block >= item->block) {
			mark = -1;
			if (next != that->link && next->stat == MEM_AVAILABLE) {
				if (mem->block + next->block >= item->block) {
					// next and prev which is the minimum
					if (next->block < prev->block) {
						mark = 1;
					}
				}
			}
		}
	}
	else if (next != that->link && next->stat == MEM_AVAILABLE) {
		if (mem->block + next->block >= item->block) {
			mark = 1;
		}
	}
	if (mark > 0) {
		item->set(item, next->block - item->block + mem->block);
		if (item->block > 0) {
			that->split(that, next, item);
		}
		else {
			//delete item;
			that->remove(that, item);
		}
		that->merge(that, mem, next);
		return addr;
	}
	else if (mark < 0) {
		item->set(item, item->block - mem->block);
		that->split(that, prev, item);
		that->merge(that, item, mem);
		item->stat = MEM_OCCUPPIED;
		return item->addr;
	}
	else {
		that->free(that, addr);
		return that->alloc(that, size);
	}

}

MEM_ADDR MemMan_alloc(MemMan * that, MEM_SIZE size) {
	if (that->link == NULL) {
		return NULL;
	}
	//MemStat * item = new MemStat(0, size);
	MemStat * item = that->memPool.get(&that->memPool);
	if (item == NULL) {
		return NULL;
	}
	item->set(item, size);
	MemStat * mem = that->link;
	MemStat * min = NULL;
	do {
		if (mem->stat == MEM_AVAILABLE && mem->block >= item->block) {
			if (min == NULL || min->block > mem->block) {
				min = mem;
			}
		}
		mem = that->next(that, mem);
	} while (mem && mem != that->link);
	if (min) {
		that->split(that, min, item);
		item->stat = MEM_OCCUPPIED;
		return item->addr;
	}
	return NULL;
}

void MemMan_free(MemMan * that, MEM_ADDR addr) {
	MemStat * mem = that->getAddr(that, addr, MEM_OCCUPPIED);
	if (mem == NULL) {
		return;
	}
	mem->stat = MEM_AVAILABLE;

	MemStat * prev = that->prev(that, mem);
	MemStat * next = that->next(that, mem);
	if (mem != that->link && prev->stat == MEM_AVAILABLE) {
		that->merge(that, prev, mem);
	}
	else if (next != that->link && next->stat == MEM_AVAILABLE) {
		that->merge(that, mem, next);
	}

	if (mem != that->link && prev->stat == MEM_AVAILABLE &&
		next != that->link && next->stat == MEM_AVAILABLE) {
		that->merge(that, prev, next);
	}
}

void MemMan_merge(MemMan * that, MemStat * start, MemStat * end) {
	if (that->link == NULL) {
		return;
	}
	if (start == NULL || end == NULL) {
		return;
	}
	if (that->next(that, start) != end) {
		return;
	}
	start->merge(start, end);
	//delete that->remove(that, end);
	that->remove(that, end);
}
MemStat * MemMan_remove(MemMan * that, MemStat * link) {
	that->removeLink(that, link);
	if (link->free(link) == NULL) {
		that->memPool.back(&that->memPool, link);
	}
	return link;
}

void MemMan_split(MemMan * that, MemStat * tango, MemStat * item) {
	if (that->link == NULL) {
		return;
	}
	if (tango == NULL || item == NULL) {
		return;
	}
	that->insertLink(that, item, NULL, tango);
	if (tango->block == item->block) {
		item->addr = tango->addr;
		//delete that->removeLink(that, tango);
		that->removeLink(that, tango);
		return;
	}
	tango->split(tango, item);
}
MemMan * _MemMan(MemMan * that, int index) {
	_MultiLinkBase(&that->super, index);
	
	that->add = MemMan_add;
	that->alloc = MemMan_alloc; 
	that->free = MemMan_free;
	that->realloc = MemMan_realloc;
	that->getAddr = MemMan_getAddr;
	that->merge = MemMan_merge;
	that->split = MemMan_split;
	that->remove = MemMan_remove;

	int i;
	for (i = 0; i < POOL_MAX; i++) {
		_MemStat(&that->pool[i], 0, 0);
	}
	_MemPool(&that->memPool, that->pool, that->map, POOL_MAX);

	return that;
}
