// KeyFIFO.h
//

#ifndef _KEYFIFO_
#define _KEYFIFO_

#include "MultiLink.h"

///////////////////////////////////////////////////////////
typedef struct KeyElm KeyElm;
struct KeyElm {
	__SUPER(MultiLinkElement, KeyElm, NULL);
	KeyElm * _prev[2];
	KeyElm * _next[2];
	char data;
};
void _Key(KeyElm * that);
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
typedef struct KeyPool KeyPool;
struct KeyPool {
	__SUPER(ElementPool, KeyPool, KeyElm);
};
KeyElm * KeyPool_at(KeyPool * that, int index);
void _KeyPool(KeyPool * that, KeyElm * pool, UMAP * map, int size);
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
typedef struct KeyFIFO KeyFIFO;
struct KeyFIFO {
	__SUPER(MultiLinkBase, KeyFIFO, KeyElm);
	KeyElm pool[POOL_MAX];
	UMAP map[MAP_MAX];
	KeyPool keyPool;
	void(*put)(KeyFIFO * that, char c);
	char(*pop)(KeyFIFO * that);
};
void KeyFIFO_put(KeyFIFO * that, char c) ;
char KeyFIFO_pop(KeyFIFO * that);
void _KeyFIFO(KeyFIFO * that, int index);
///////////////////////////////////////////////////////////

#endif