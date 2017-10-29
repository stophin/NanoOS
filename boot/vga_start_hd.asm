[bits 16]
; remember VGA mode
VMODE	equ	0x0ff0	; VGA mode
SCRNX	equ	0x0ff2	; screen X 
SCRNY	equ	0x0ff4	; screen Y
VRAM	equ	0x0ff8	; memory cache
; vga start
vga_start:
	mov bx, 0x4101	; VBE card, 640*480*8bit
					; Video Electronics Standards Association-BIOS Extension
					; VBE display mode(BX)
					; 0x101 640*480*8bit
					; 0x103 800*600*8bit
					; 0x105 1024*768*8bit
					; 0x107 1280*1024*8bit
	mov ax, 0x4f02
	int 0x10


	mov	byte [VMODE], 8
	mov	word [SCRNX], 640
	mov	word [SCRNY], 480
	mov	dword [VRAM], 0xe0000000
	
	ret