// KeyFIFO.h
//

#pragma once

#include "MultiLink.h"

typedef struct Key Key;
struct Key {
	__SUPER(MultiLinkElement, Key, NULL);
	Key * _prev[2];
	Key * _next[2];
	char data;
};
void _Key(Key * that) {
	that->prev = that->_prev;
	that->next = that->_next;
	_MultiLinkElement(&that->super, 2);

	that->data = '\0';
}

typedef struct KeyPool KeyPool;
struct KeyPool {
	__SUPER(ElementPool, KeyPool, Key);
};
Key * KeyPool_at(KeyPool * that, int index) {
	return &that->pool[index];
}
void _KeyPool(KeyPool * that, Key * pool, UMAP * map, int size) {
	_ElementPool(&that->super, (MultiLinkElement *)pool, map, size);

	that->at = KeyPool_at;
}

typedef struct KeyFIFO KeyFIFO;
struct KeyFIFO {
	__SUPER(MultiLinkBase, KeyFIFO, Key);
	Key pool[POOL_MAX];
	UMAP map[MAP_MAX];
	KeyPool keyPool;
	void(*put)(KeyFIFO * that, char c);
	char(*pop)(KeyFIFO * that);
};
void KeyFIFO_put(KeyFIFO * that, char c) {
	Key * key = that->keyPool.get(&that->keyPool);
	if (key == NULL) {
		return;
	}
	key->data = c;
	that->insertLink(that, key, NULL, NULL);
}
char KeyFIFO_pop(KeyFIFO * that) {
	Key * key = that->removeLink(that, that->link);
	if (key == NULL) {
		return '\0';
	}
	char c = key->data;
	if (key->free(key) == NULL) {
		that->keyPool.back(&that->keyPool, key);
	}
	return c;
}
void _KeyFIFO(KeyFIFO * that, int index) {
	_MultiLinkBase(&that->super, index);
	int i;
	for (i = 0; i < POOL_MAX; i++) {
		_Key(&that->pool[i]);
	}
	_KeyPool(&that->keyPool, that->pool, that->map, POOL_MAX);

	that->put = KeyFIFO_put;
	that->pop = KeyFIFO_pop;
}