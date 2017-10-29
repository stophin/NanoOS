[bits 16]
; remember VGA mode
VMODE	equ	0x0ff0	; VGA mode
SCRNX	equ	0x0ff2	; screen X 
SCRNY	equ	0x0ff4	; screen Y
VRAM	equ	0x0ff8	; memory cache
; vga start
vga_start:

	mov	al, 0x13	; VGA card, 320*240*8bit
					; other:
					; 0x03: 16bit character 80 * 25, initial mode
					; 0x12: VGA card, 640*480*4bit
					; 0x6a: extended VGA card, 800*600*4
	mov	ah, 0x00
	int	0x10
	
	mov	byte [VMODE], 8
	mov	word [SCRNX], 320
	mov	word [SCRNY], 200
	mov	dword [VRAM], 0xa0000
	
	ret