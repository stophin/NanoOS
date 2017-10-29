// KeyFIFO.h
//

#include "KeyFIFO.h"

///////////////////////////////////////////////////////////
void _KeyElm(KeyElm * that) {
	that->prev = that->_prev;
	that->next = that->_next;
	_MultiLinkElement(&that->super, 2);

	that->data = '\0';
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
KeyElm * KeyPool_at(KeyPool * that, int index) {
	return &that->pool[index];
}
void _KeyPool(KeyPool * that, KeyElm * pool, UMAP * map, int size) {
	_ElementPool(&that->super, (MultiLinkElement *)pool, map, size);

	that->at = KeyPool_at;
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void KeyFIFO_put(KeyFIFO * that, char c) {
	KeyElm * key = that->keyPool.get(&that->keyPool);
	if (key == NULL) {
		return;
	}
	key->data = c;
	that->insertLink(that, key, NULL, NULL);
}
char KeyFIFO_pop(KeyFIFO * that) {
	char c;
	KeyElm * key = that->removeLink(that, that->link);
	if (key == NULL) {
		return '\0';
	}
	c = key->data;
	if (key->free(key) == NULL) {
		that->keyPool.back(&that->keyPool, key);
	}
	return c;
}
void _KeyFIFO(KeyFIFO * that, int index) {
	int i;
	_MultiLinkBase(&that->super, index);
	for (i = 0; i < POOL_MAX; i++) {
		_KeyElm(&that->pool[i]);
	}
	_KeyPool(&that->keyPool, that->pool, that->map, POOL_MAX);

	that->put = KeyFIFO_put;
	that->pop = KeyFIFO_pop;
}
///////////////////////////////////////////////////////////