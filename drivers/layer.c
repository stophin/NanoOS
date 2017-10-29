// layer.c
// author: stophin
//
#include "../drivers/layer.h"

// layers
LAYER_CTL * layer_ctl_lay(LAYER_CTL * ctl, BYTE * vram, INT xsize, INT ysize) {
	INT i;
	ctl->vram = vram;
	ctl->xsize = xsize;
	ctl->ysize = ysize;
	ctl->top = -1;				// not any layer
	for (i = 0; i < MAX_LAYERS; i ++) {
		ctl->layer_base[i].flags = LAYER_UNUSED;	// marked as unused
	}
	return ctl;
}

// layers
LAYER_CTL * layer_ctl_init(MEMMAN * man, BYTE * vram, INT xsize, INT ysize) {
	LAYER_CTL * ctl;
	INT i;
	ctl = (LAYER_CTL *)memman_alloc_4k(man, sizeof(LAYER_CTL));
	if (ctl == 0) {
		return ctl;
	}
	ctl->map = (BYTE *)memman_alloc_4k(man, xsize * ysize);
	if (ctl->map == 0) {
		memman_free_4k(man, (DWORD)ctl, sizeof(LAYER_CTL));
		return 0;
	}
	ctl->vram = vram;
	ctl->xsize = xsize;
	ctl->ysize = ysize;
	ctl->top = -1;				// not any layer
	for (i = 0; i < MAX_LAYERS; i ++) {
		ctl->layer_base[i].flags = LAYER_UNUSED;	// marked as unused
	}
	return ctl;
}

LAYER * layer_alloc(LAYER_CTL * ctl) {
	LAYER * lay;
	INT i;
	for (i = 0; i < MAX_LAYERS; i ++) {
		if (ctl->layer_base[i].flags == LAYER_UNUSED) {
			lay = &ctl->layer_base[i];
			lay->flags = LAYER_USED;		// marked as used
			lay->height = -1;				// hidden
			return lay;
		}
	}
	return 0;	// all layers are used
}

void layer_set(LAYER * lay, BYTE * buf, INT xsize, INT ysize, BYTE col_inv) {
	lay->buf = buf;
	lay->bxsize = xsize;
	lay->bysize = ysize;
	lay->col_inv = col_inv;
	lay->vx0 = 0;
	lay->vy0 = 0;
	lay->mx0 = 0;
	lay->my0 = 0;
	return;
}

void layer_height(LAYER_CTL * ctl, LAYER * lay, INT height) {
	INT h, old = lay->height; // remember old height

	// cut height from -1 to top
	if (height > ctl->top + 1) {
		height = ctl->top + 1;
	}
	if (height < -1) {
		height = -1;
	}
	// set height
	lay->height = height;

	// resort layers' height
	if (old > height) {	// lower than before
		if (height >= 0) {
			// move up the layers in middle
			for (h = old; h > height; h --) {
				ctl->layers[h] = ctl->layers[h - 1];
				ctl->layers[h]->height = h;
			}
			ctl->layers[height] = lay;
			//layer_refresh(ctl);	// refresh layers
			layer_refresh_region(ctl, lay->vx0, lay->vy0, lay->vx0 + lay->bxsize, lay->vy0 + lay->bysize, height + 1, -1);
		} else {	// hide
			if (ctl->top > old) {
				// move down the layers at top
				for (h = old; h < ctl->top; h ++) {
					ctl->layers[h] = ctl->layers[h + 1];
					ctl->layers[h]->height = h;
				}
			}
			ctl->top --;	// hidden, layer - 1
			//layer_refresh(ctl);	// refresh layers
			layer_refresh_region(ctl, lay->vx0, lay->vy0, lay->vx0 + lay->bxsize, lay->vy0 + lay->bysize, 0, -1);
		}
	} else if (old < height) {	// higher than before
		if (old >= 0) {
			// move down the layers in middle
			for (h = old; h < height; h ++) {
				ctl->layers[h] = ctl->layers[h + 1];
				ctl->layers[h]->height = h;
			}
			ctl->layers[height] = lay;
		} else {	// hidden -> show
			// move up the layers at top
			for (h = ctl->top; h >= height; h --) {
				ctl->layers[h + 1] = ctl->layers[h];
				ctl->layers[h + 1]->height = h + 1;
			}
			ctl->layers[height] = lay;
			ctl->top ++;	// show, layer + 1
		}
		//layer_refresh(ctl);	// refresh layers
		sprintf(100, 100, COL_848484, COL_FFFFFF, 1, "Here");
		layer_refresh_region(ctl, lay->vx0, lay->vy0, lay->vx0 + lay->bxsize, lay->vy0 + lay->bysize, height, -1);
	}
	return;
}

void layer_refresh(LAYER_CTL * ctl, LAYER * lay, INT bx0, INT by0, INT bx1, INT by1) {	
	if (lay->height >= 0) {		// is shown
		layer_refresh_region(ctl, lay->vx0 + bx0, lay->vy0 + by0, lay->vx0 + bx1, lay->vy0 + by1, lay->height, lay->height);
	}
	return;
}

void layer_refresh_map(LAYER_CTL * ctl, INT vx0, INT vy0, INT vx1, INT vy1, INT height) {
	INT h, bx, by, vx, vy, bx0, by0, bx1, by1;
	BYTE * buf, c, * vram = ctl->vram, * map = ctl->map, sid;
	LAYER * lay;
	// out of screen
	if (vx0 < 0) {
		vx0 = 0;
	}
	if (vy0 < 0) {
		vy0 = 0;
	}
	if (vx1 > ctl->xsize) {
		vx1 = ctl->xsize;
	}
	if (vy1 > ctl->ysize) {
		vy1 = ctl->ysize;
	}
	for (h = height; h <= ctl->top; h ++) {
		lay = ctl->layers[h];
		sid = lay - ctl->layer_base;
		buf = lay->buf;
		// vx0~vy1 -> bx0~by1
		bx0 = vx0 - lay->vx0;
		by0 = vy0 - lay->vy0;
		bx1 = vx1 - lay->vx0;
		by1 = vy1 - lay->vy0;
		if (bx0 < 0) {
			bx0 = 0;
		}
		if (by0 < 0) {
			by0 = 0;
		}
		if (bx1 > lay->bxsize) {
			bx1 = lay->bxsize;
		}
		if (by1 > lay->bysize) {
			by1 = lay->bysize;
		}
		for (by = by0; by < by1; by ++) {
			vy = lay->vy0 + by;
			for (bx = bx0; bx < bx1; bx ++) {
				vx = lay->vx0 + bx;
				if (buf[by * lay->bxsize + bx] != lay->col_inv) {
				//if (1) {
					map[vy * ctl->xsize + vx] = sid;
				}
			}
		}
	}
	return;
}


void layer_refresh_region(LAYER_CTL * ctl, INT vx0, INT vy0, INT vx1, INT vy1, INT height, INT top) {
	INT h, bx, by, vx, vy, bx0, by0, bx1, by1;
	BYTE * buf, c, * vram = ctl->vram, * map = ctl->map, sid;
	LAYER * lay;
	// out of screen
	if (vx0 < 0) {
		vx0 = 0;
	}
	if (vy0 < 0) {
		vy0 = 0;
	}
	if (vx1 > ctl->xsize) {
		vx1 = ctl->xsize;
	}
	if (vy1 > ctl->ysize) {
		vy1 = ctl->ysize;
	}
	if (top == -1) {
		top = ctl->top;
	}
	for (h = height; h <= top; h ++) {
		lay = ctl->layers[h];
		buf = lay->buf;
		sid = lay - ctl->layer_base;
		// vx0~vy1 -> bx0~by1
		bx0 = vx0 - lay->vx0;
		by0 = vy0 - lay->vy0;
		bx1 = vx1 - lay->vx0;
		by1 = vy1 - lay->vy0;
		if (bx0 < 0) {
			bx0 = 0;
		}
		if (by0 < 0) {
			by0 = 0;
		}
		if (bx1 > lay->bxsize) {
			bx1 = lay->bxsize;
		}
		if (by1 > lay->bysize) {
			by1 = lay->bysize;
		}
		for (by = by0; by < by1; by ++) {
			vy = lay->vy0 + by;
			for (bx = bx0; bx < bx1; bx ++) {
				vx = lay->vx0 + bx;
				if (map[vy * ctl->xsize + vx] == sid) {
				//if (1) {
					vram[vy * ctl->xsize + vx] = buf[by * lay->bxsize + bx];
					//vram[vy * ctl->xsize + vx] = map[vy * ctl->xsize + vx];
				} else {
					
					//vram[vy * ctl->xsize + vx] = COL_848484;
				}
			}
		}
	}
	return;
}

void layer_free(LAYER_CTL * ctl, LAYER * lay) {
	if (lay->height >= 0) {
		layer_height(ctl, lay, -1);	// hide
	}
	lay->flags = 0;
	return;
}

void layer_slide(LAYER_CTL * ctl, LAYER * lay, INT vx0, INT vy0) {
	INT old_vx0 = lay->vx0, old_vy0 = lay->vy0;
	lay->vx0 = vx0;
	lay->vy0 = vy0;
	if (lay->height >= 0) {	// if it is shown
		//layer_refresh(ctl);	// refresh layers
		layer_refresh_map(ctl, old_vx0, old_vy0, old_vx0 + lay->bxsize, old_vy0 + lay->bysize, 0);
		layer_refresh_map(ctl, vx0, vy0, vx0 + lay->bxsize, vy0 + lay->bysize, lay->height);
		layer_refresh_region(ctl, old_vx0, old_vy0, old_vx0 + lay->bxsize, old_vy0 + lay->bysize, 0, lay->height - 1);
		layer_refresh_region(ctl, vx0, vy0, vx0 + lay->bxsize, vy0 + lay->bysize, lay->height, lay->height);
	}
	return;
}

void layer_string(LAYER_CTL * ctl, LAYER * lay, INT x, INT y, BYTE c, BYTE b, BYTE scl, BYTE * str) {
	INT i = 0;
	for (i = 0; str[i] != '\0'; i ++) {
		boxfill_b(lay->buf, lay->bxsize, b, x + FONT_W * scl * i, y, x + FONT_W * scl * (i + 1), y + FONT_H * scl);
		putfont_b(lay->buf, lay->bxsize, x + FONT_W * scl * i, y, c, scl, sys_font + str[i] * FONT_H);
	}
	layer_refresh_map(ctl, lay->vx0 + x , lay->vy0 + y, lay->vx0 + x + FONT_W * scl * i, lay->vy0 + y + FONT_H * scl, lay->height);
	layer_refresh_region(ctl, lay->vx0 + x , lay->vy0 + y, lay->vx0 + x + FONT_W * scl * i, lay->vy0 + y + FONT_H * scl, lay->height, lay->height);
}

BYTE isin(LAYER * tag, LAYER * itm) {
	BYTE result = 0;
	if (itm->vx0 > tag->vx0 && itm->vx0 < tag->vx0 + tag->bxsize) {
		if (itm->vy0 > tag->vy0 && itm->vy0 < tag->vy0 + tag->bysize) {
			result = 1;
		}
	}
	return result;
}


void draw_back(LAYER * lay) {
	INT x, y, i;

	x = lay->bxsize;
	y = lay->bysize;
	boxfill_b(lay->buf, x, COL_008484,  0,          0, x -  1, y - 29);
	boxfill_b(lay->buf, x, COL_C6C6C6,  0,     y - 28, x -  1, y - 28);
	boxfill_b(lay->buf, x, COL_FFFFFF,  0,     y - 27, x -  1, y - 27);
	boxfill_b(lay->buf, x, COL_C6C6C6,  0,     y - 26, x -  1, y -  1);

	boxfill_b(lay->buf, x, COL_FFFFFF,  3,     y - 24, 59,     y - 24);
	boxfill_b(lay->buf, x, COL_FFFFFF,  2,     y - 24,  2,     y -  4);
	boxfill_b(lay->buf, x, COL_848484,  3,     y -  4, 59,     y -  4);
	boxfill_b(lay->buf, x, COL_848484, 59,     y - 23, 59,     y -  5);
	boxfill_b(lay->buf, x, COL_000000,  2,     y -  3, 59,     y -  3);
	boxfill_b(lay->buf, x, COL_000000, 60,     y - 24, 60,     y -  3);

	boxfill_b(lay->buf, x, COL_848484, x - 47, y - 24, x -  4, y - 24);
	boxfill_b(lay->buf, x, COL_848484, x - 47, y - 23, x - 47, y -  4);
	boxfill_b(lay->buf, x, COL_FFFFFF, x - 47, y -  3, x -  4, y -  3);
	boxfill_b(lay->buf, x, COL_FFFFFF, x -  3, y - 24, x -  3, y -  3);
	
	i = 1;
	putstring_b(lay->buf, x, 20 * FONT_W, 10 + FONT_H * i++, COL_FFFFFF, 2, "Welcome to OS!");
}

void draw_window(LAYER * lay) {
	INT x, y;

	x = lay->bxsize;
	y = lay->bysize;
	boxfill_b(lay->buf, x, COL_C6C6C6,  0,          0, x -  1,      0);
	boxfill_b(lay->buf, x, COL_FFFFFF,  1,          1, x -  2,      1);
	boxfill_b(lay->buf, x, COL_C6C6C6,  0,          0,      0, y -  1);
	boxfill_b(lay->buf, x, COL_FFFFFF,  1,          1,      1, y -  2);

	boxfill_b(lay->buf, x, COL_848484, x -  2,      1, x -  2, y -  2);
	boxfill_b(lay->buf, x, COL_000000, x -  1,      0, x -  1, y -  1);
	boxfill_b(lay->buf, x, COL_C6C6C6,      2,      2, x -  3, y -  3);
	boxfill_b(lay->buf, x, COL_000084,      3,      3, x -  4,     20);
	boxfill_b(lay->buf, x, COL_848484,  1,     y -  2, x -  2, y -  2);
	boxfill_b(lay->buf, x, COL_000000,  0,     y -  1, x -  1, y -  1);
	
	putstring_b(lay->buf, x, 24, 4, COL_FFFFFF, 1, "Hello OS!");
}