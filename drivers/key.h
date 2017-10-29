// key.h
// author: stophin
//

void int_handler21();
void wait_sendready();
void init_key();
BYTE decode_key(BYTE data);

typedef struct Key {
	INT code;
	BYTE *ascii_tab;
	BYTE *ascii_shift_tab;
} Key;