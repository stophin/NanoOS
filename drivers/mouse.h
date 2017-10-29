// mouse.h
// author: stophin
//

void int_handler2c();
void init_mouse();
int decode_mouse(BYTE data);

typedef struct Mouse {
	INT x;
	INT y;
	INT vx;
	INT vy;
	BYTE button;
	BYTE phase;
	BYTE buff[3];
	BYTE state;
} Mouse;

extern Mouse mouse;