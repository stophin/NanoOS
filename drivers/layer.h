// layer.h
// author: stophin
//
#include "../lib/type.h"

#include "../drivers/screen.h"
#include "../memory/memory.h"

#ifndef _LAYER_
#define _LAYER_

// layers
#define MAX_LAYERS	256

#define LAYER_UNUSED	0
#define LAYER_USED		1

typedef struct LAYER {
	BYTE * buf;
	INT bxsize, bysize;
	INT mx0, my0;
	INT vx0, vy0;
	BYTE col_inv;		// transparent color
	INT height;			// depth
	INT flags;			// layer state
} LAYER;

typedef struct LAYER_CTL {
	BYTE * vram, * map;
	INT xsize, ysize, top;
	LAYER * layers[MAX_LAYERS];
	LAYER layer_base[MAX_LAYERS];
}LAYER_CTL;


LAYER_CTL * layer_ctl_lay(LAYER_CTL * lay, BYTE * vram, INT xsize, INT ysize);
LAYER_CTL * layer_ctl_init(MEMMAN * man, BYTE * vram, INT xsize, INT ysize);
LAYER * layer_alloc(LAYER_CTL * ctl);
void layer_set(LAYER * lay, BYTE * buf, INT xsize, INT ysize, BYTE col_inv) ;
void layer_height(LAYER_CTL * ctl, LAYER * lay, INT height);
//void layer_refresh(LAYER_CTL * ctl);
void layer_refresh(LAYER_CTL * ctl, LAYER * lay, INT bx0, INT by0, INT bx1, INT by1);
void layer_refresh_map(LAYER_CTL * ctl, INT vx0, INT vy0, INT vx1, INT vy1, INT height);
void layer_refresh_region(LAYER_CTL * ctl, INT vx0, INT vy0, INT vx1, INT vy1, INT height, INT top);
void layer_slide(LAYER_CTL * ctl, LAYER * lay, INT vx0, INT vy0);
void layer_free(LAYER_CTL * ctl, LAYER * lay);

void layer_string(LAYER_CTL * ctl, LAYER * lay, INT x, INT y, BYTE c, BYTE b, BYTE scl, BYTE * str);

void draw_back(LAYER * lay);
void draw_window(LAYER * lay);

BYTE isin(LAYER * tag, LAYER * itm);

#endif