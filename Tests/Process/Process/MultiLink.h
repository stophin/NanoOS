// MultiLink.h
//

#pragma once

#include <stdlib.h>

#define __SUPER(B, T, E) \
	union {\
		B super; \
		struct {\
			Template##B (T, E);\
		}; \
	}

//////////////////////////////////////////////////////////

struct MultiLinkElement {
#define MultiLinkElementTemplate(T)\
	int linkcount;\
	T ** prev;\
	T ** next;\
	void(*final)(T *that);\
	T * (*free)(T * that);\
	void(*clear)(T * that);
#define TemplateMultiLinkElement(T, E) MultiLinkElementTemplate(struct T)
	TemplateMultiLinkElement(MultiLinkElement, NULL);
};
void MultiLinkElement_clear(MultiLinkElement * that) {
	int i;
	for (i = 0; i < that->linkcount; i++) {
		that->prev[i] = NULL;
		that->next[i] = NULL;
	}
}
MultiLinkElement * MultiLinkElement_free(MultiLinkElement * that) {
	int i;
	for (i = 0; i < that->linkcount; i++) {
		if (that->prev[i] != NULL || that->next[i] != NULL) {
			return that;
		}
	}
	return NULL;
}
void _MultiLinkElement(MultiLinkElement * that, int linkcount) {
	that->linkcount = linkcount;

	that->clear = MultiLinkElement_clear;
	that->free = MultiLinkElement_free;
	that->final = NULL;

	that->clear(that);
}
//////////////////////////////////////////////////////////
struct MultiLinkBase {
#define MultiLinkBaseTemplate(T, E) \
	int linkcount;\
	int linkindex;\
	E * link;\
	void(*insertLink)(T * that, E * link, E * before, E * after); \
	E * (*removeLink)(T * that, E * link); \
	E * (*get)(T * that, int index); \
	E * (*prev)(T *that, E * link); \
	E * (*next)(T *that, E * link);
#define TemplateMultiLinkBase(T, E) MultiLinkBaseTemplate(struct T, struct E)
	TemplateMultiLinkBase(MultiLinkBase, MultiLinkElement);
};
MultiLinkElement * MultiLinkBase_removeLink(MultiLinkBase * that, MultiLinkElement * link) {
	if (link == NULL)
	{
		return NULL;
	}
	if (that->linkindex < 0)
	{
		return NULL;
	}
	if (link->prev[that->linkindex] == NULL || link->next[that->linkindex] == NULL)
	{
		return NULL;
	}
	MultiLinkElement * before = link->prev[that->linkindex];
	MultiLinkElement * after = link->next[that->linkindex];

	before->next[that->linkindex] = after;
	after->prev[that->linkindex] = before;
	link->prev[that->linkindex] = NULL;
	link->next[that->linkindex] = NULL;

	if (that->link == link)
	{
		that->link = after;
	}
	if (that->link == link)
	{
		that->link = NULL;
	}

	that->linkcount = that->linkcount - 1;

	return link;
}
MultiLinkElement * MultiLinkBase_get(MultiLinkBase * that, int index) {
	if (that->link == NULL)
	{
		return NULL;
	}
	MultiLinkElement * temp = that->link;
	do
	{
		temp = temp->next[that->linkindex];
	} while (temp && temp != that->link && --index);
	return temp;
}

void MultiLinkBase_insertLink(MultiLinkBase * that, MultiLinkElement * link, MultiLinkElement * before, MultiLinkElement * after) {
	if (link == NULL)
	{
		return;
	}
	if (that->link == NULL)
	{
		that->link = link;

		that->link->prev[that->linkindex] = link;
		that->link->next[that->linkindex] = link;

		that->linkcount = that->linkcount + 1;

		return;
	}
	else
	{
		MultiLinkElement * _link = NULL;
		if (before == that->link)
		{
			_link = link;
		}
		if (before == NULL && after == NULL)
		{
			before = that->link;
			after = that->link->prev[that->linkindex];
		}
		else if (before == NULL)
		{
			before = after->next[that->linkindex];
		}
		else if (after == NULL)
		{
			after = before->prev[that->linkindex];
		}
		else /* before != NULL && after != NULL*/
		{
			if (before->prev[that->linkindex] != after || after->next[that->linkindex] != before)
			{
				return;
			}
		}
		if (before == NULL || after == NULL ||
			before->prev[that->linkindex] == NULL ||
			after->next[that->linkindex] == NULL)
		{
			return;
		}

		link->prev[that->linkindex] = after;
		link->next[that->linkindex] = before;
		after->next[that->linkindex] = link;
		before->prev[that->linkindex] = link;

		if (_link)
		{
			that->link = _link;
		}

		that->linkcount = that->linkcount + 1;
	}
}
MultiLinkElement * MultiLinkBase_prev(MultiLinkBase *that, MultiLinkElement * link) {
	if (link == NULL)
	{
		return NULL;
	}
	return link->prev[that->linkindex];
}
MultiLinkElement * MultiLinkBase_next(MultiLinkBase *that, MultiLinkElement * link) {
	if (link == NULL)
	{
		return NULL;
	}
	return link->next[that->linkindex];
}
void _MultiLinkBase(MultiLinkBase * that, int linkindex) {
	that->linkcount = 0;
	that->linkindex = linkindex;
	that->link = NULL;

	that->insertLink = MultiLinkBase_insertLink;
	that->prev = MultiLinkBase_prev;
	that->next = MultiLinkBase_next;
	that->removeLink = MultiLinkBase_removeLink;
	that->get = MultiLinkBase_get;
}
////////////////////////////////////////////////////////
typedef unsigned char UMAP;
#define MAP_SHIFT	8
#define POOL_MAX	10
#define MAP_MAX	POOL_MAX / MAP_SHIFT + 1
#define MAP_MASK	0xFF

struct ElementPool {
#define ElementPoolTemplate(T, E)\
	E * pool;\
	UMAP * map;\
	int size;\
	int msize;\
	int count;\
	E * (*at)(T * that, int index);\
	E * (*get)(T * that);\
	void(*back)(T * that, E * o);
#define TemplateElementPool(T, E) ElementPoolTemplate(struct T, struct E)
	TemplateElementPool(ElementPool, MultiLinkElement);
};
MultiLinkElement * ElementPool_at(ElementPool * that, int index) {
	// Inherit struct must override this function
	// because the size of the type of pool is different
	// Note: in this kind of inherit, be careful when
	// using arry of specified type, but there's no
	// need to worry about using pointer of the type
	// e.g. MultiLinkElement has pointer array :
	// prev and next, and there's no need to
	// override any get/set function in inherit struct
	return &that->pool[index];
}
MultiLinkElement * ElementPool_get(ElementPool * that) {
	int i, j, index;
	for (i = 0, index = 0; i < that->msize && index < that->size; i++, index += MAP_SHIFT) {
		if (that->map[i] & MAP_MASK) {
			for (j = 0; j < MAP_SHIFT && index < that->size; j++, index++) {
				if (that->map[i] & (0x01 << j)) {
					that->map[i] &= ~(0x01 << j);
					return that->at(that, index);
				}
			}
		}
	}
	return NULL;
}
void ElementPool_back(ElementPool * that, MultiLinkElement * o){
	if (o == NULL) {
		return;
	}
	int i, j, index;
	for (index = 0; index < that->size; index++) {
		if (that->at(that, index) == o) {
			i = index / MAP_MASK;
			j = index - i * MAP_MASK;
			that->map[i] |= (0x01 << j);
			if (o->final) {
				o->final(o);
			}
			return;
		}
	}
}
void _ElementPool(ElementPool * that, MultiLinkElement * pool, UMAP * map, int size) {
	if (size > POOL_MAX) {
		size = POOL_MAX;
	}
	that->pool = pool;
	that->map = map;
	that->size = size;

	that->at = ElementPool_at;
	that->get = ElementPool_get;
	that->back = ElementPool_back;

	that->msize = size / MAP_SHIFT + 1;
	if (that->msize > MAP_MAX) {
		that->msize = MAP_MAX;
	}

	int i;
	for (i = 0; i < that->msize; i++) {
		that->map[i] = MAP_MASK;
	}
}
//////////////////////////////////////////////////////////
typedef struct Task Task;
struct Task {
	__SUPER(MultiLinkElement, Task, NULL);
	Task * _prev[2];
	Task * _next[2];
	int data;
};
void _Task(Task * that, int data) {
	that->prev = that->_prev;
	that->next = that->_next;
	_MultiLinkElement(&that->super, 2);

	that->data = data;
}
////////////////////////////////////////////////////////
typedef struct TaskPool TaskPool;
struct TaskPool {
	__SUPER(ElementPool, TaskPool, Task);
};
Task * TaskPool_at(TaskPool * that, int index) {
	return &that->pool[index];
}
void _TaskPool(TaskPool * that, Task * pool, UMAP * map, int size) {
	_ElementPool(&that->super, (MultiLinkElement *)pool, map, size);

	that->at = TaskPool_at;
}
////////////////////////////////////////////////////////
typedef struct TaskControl TaskControl;
struct TaskControl {
	__SUPER(MultiLinkBase, TaskControl, Task);
	Task tasks[POOL_MAX];
	UMAP map[MAP_MAX];
	TaskPool taskPool;
	void(*add)(TaskControl * that, Task * link, Task * before, Task * after);
};
TaskControl * _TaskControl(TaskControl * that) {
	_MultiLinkBase(&that->super, 0);
	int i;
	for (i = 0; i < POOL_MAX; i++) {
		_Task(&that->tasks[i], 0);
	}
	_TaskPool(&that->taskPool, that->tasks, that->map, POOL_MAX);

	return that;
}