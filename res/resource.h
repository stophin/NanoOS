// resource.h
// author: stophin
//
#include "../lib/type.h"

// including all kinds of arrays

// for screen color palette
extern BYTE table_rgb[16 * 3];
// for mouse
BYTE cursor[17][11];
extern BYTE mouse_buff[100];
// for key
BYTE ascii_tab[58];
BYTE ascii_shift_tab[58];
extern BYTE key_buff[100];

// temporary char array
extern BYTE buffer[100];
extern BYTE buffer_t[100];

// exceptions
extern BYTE error_desc[][100];