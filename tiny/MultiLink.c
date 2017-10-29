// MultiLink.h
//

#include "MultiLink.h"

///////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
MultiLinkElement * MultiLinkBase_removeLink(MultiLinkBase * that, MultiLinkElement * link) {
	
	MultiLinkElement * before, * after;
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
	before = link->prev[that->linkindex];
	after = link->next[that->linkindex];

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
	MultiLinkElement * temp;
	if (that->link == NULL)
	{
		return NULL;
	}
	temp = that->link;
	do
	{
		temp = temp->next[that->linkindex];
	} while (temp && temp != that->link && --index);
	return temp;
}

MultiLinkElement * MultiLinkBase_insertLink(MultiLinkBase * that, MultiLinkElement * link, MultiLinkElement * before, MultiLinkElement * after) {
	MultiLinkElement * _link;
	if (link == NULL)
	{
	//	return link;
	}
	if (that->link == NULL)
	{
		that->link = link;

		that->link->prev[that->linkindex] = link;
		that->link->next[that->linkindex] = link;

		that->linkcount = that->linkcount + 1;

		return link;
	}
	else
	{
		_link = NULL;
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
				return link;
			}
		}
		if (before == NULL || after == NULL ||
			before->prev[that->linkindex] == NULL ||
			after->next[that->linkindex] == NULL)
		{
			return link;
		}

		if (link) {
		link->prev[that->linkindex] = after;
		link->next[that->linkindex] = before;
		}
		after->next[that->linkindex] = link;
		before->prev[that->linkindex] = link;

		if (_link)
		{
			that->link = _link;
		}

		that->linkcount = that->linkcount + 1;
	}
	return link;
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
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
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
	int i, j, index;
	if (o == NULL) {
		return;
	}
	for (index = 0; index < that->size; index++) {
		if (that->at(that, index) == o) {
			i = index / MAP_SHIFT;
			j = index - i * MAP_SHIFT;
			that->map[i] |= (0x01 << j);
			return;
		}
	}
}
void _ElementPool(ElementPool * that, MultiLinkElement * pool, UMAP * map, int size) {
	int i;
	if (size > POOL_MAX) {
	//	size = POOL_MAX;
	}
	that->pool = pool;
	that->map = map;
	that->size = size;

	that->at = ElementPool_at;
	that->get = ElementPool_get;
	that->back = ElementPool_back;

	that->msize = size / MAP_SHIFT + 1;
	if (that->msize > MAP_MAX) {
	//	that->msize = MAP_MAX;
	}

	for (i = 0; i < that->msize; i++) {
		that->map[i] = MAP_MASK;
	}
}
///////////////////////////////////////////////////////////
