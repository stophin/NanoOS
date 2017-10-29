// MultiLink.h
//

#ifndef _MULTILINK_
#define _MULTILINK_

#include "../lib/type.h"

//////////////////////////////////////////////////////////
#define __SUPER(B, T, E) \
	union {\
		B super; \
		struct {\
			Template##B (T, E)\
		}; \
	}
//////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
typedef struct MultiLinkElement {
#define MultiLinkElementTemplate(T)\
	int linkcount;\
	T ** prev;\
	T ** next;\
	void(*final)(T *that);\
	T * (*free)(T * that);\
	void(*clear)(T * that);
#define TemplateMultiLinkElement(T, E) MultiLinkElementTemplate(struct T)
	TemplateMultiLinkElement(MultiLinkElement, NULL)
}MultiLinkElement;
void MultiLinkElement_clear(MultiLinkElement * that);
MultiLinkElement * MultiLinkElement_free(MultiLinkElement * that);
void _MultiLinkElement(MultiLinkElement * that, int linkcount);
///////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
typedef struct MultiLinkBase {
#define MultiLinkBaseTemplate(T, E) \
	int linkcount;\
	int linkindex;\
	E * link;\
	E * (*insertLink)(T * that, E * link, E * before, E * after); \
	E * (*removeLink)(T * that, E * link); \
	E * (*get)(T * that, int index); \
	E * (*prev)(T *that, E * link); \
	E * (*next)(T *that, E * link);
#define TemplateMultiLinkBase(T, E) MultiLinkBaseTemplate(struct T, struct E)
	TemplateMultiLinkBase(MultiLinkBase, MultiLinkElement)
}MultiLinkBase;
MultiLinkElement * MultiLinkBase_removeLink(MultiLinkBase * that, MultiLinkElement * link);
MultiLinkElement * MultiLinkBase_get(MultiLinkBase * that, int index);
MultiLinkElement * MultiLinkBase_insertLink(MultiLinkBase * that, MultiLinkElement * link, MultiLinkElement * before, MultiLinkElement * after) ;
MultiLinkElement * MultiLinkBase_prev(MultiLinkBase *that, MultiLinkElement * link) ;
MultiLinkElement * MultiLinkBase_next(MultiLinkBase *that, MultiLinkElement * link);
void _MultiLinkBase(MultiLinkBase * that, int linkindex);
////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
typedef unsigned char UMAP;
#define MAP_SHIFT	8
#define MAP_MASK	0xFF
#define POOL_MAX	10
#define GET_MAP_SIZE(x) (x / MAP_SHIFT + 1)
#define MAP_MAX	GET_MAP_SIZE(POOL_MAX)

typedef struct ElementPool {
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
	TemplateElementPool(ElementPool, MultiLinkElement)
}ElementPool;
MultiLinkElement * ElementPool_at(ElementPool * that, int index);
MultiLinkElement * ElementPool_get(ElementPool * that);
void ElementPool_back(ElementPool * that, MultiLinkElement * o);
void _ElementPool(ElementPool * that, MultiLinkElement * pool, UMAP * map, int size);
///////////////////////////////////////////////////////////

#endif
