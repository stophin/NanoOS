; loader.asm
[org  0x8000]	; where loader will be loaded to
KERNEL_OFFSET	equ	0x9000		; this is the memory offset to which we have loaded our kernel
KERNEL_REAL		equ	0x1000		; this is the real memory addres to which we started our kernel
LOAD_CYLINDERS	equ	10
								; total size = 512 * cycliners * head(2) * 18 (sectors per cylinder)
KERNEL_SIZE		equ 512 * LOAD_CYLINDERS * 2 * 18

LOADER_OFFSET equ 0x8000
LOADER_REAL   equ 0x90000
LOADER_SIZE   equ 512
[bits 32]
; this is where we arrive after switching to and initialising protected mode.
BEGIN_LOADER:

	mov	ebx, LOADER_MSG
	call print_string_pm		; use out 32-bit print routine.

    ; move loader itself to the sepcified place
	mov ecx, LOADER_SIZE
	mov esi, LOADER_OFFSET
	mov edi, LOADER_REAL
copy_loader:
	cmp ecx, 0
	je copy_loader_End

	mov al, byte [es:esi]; buffer address
	mov byte [ds:edi], al

	inc edi
	inc esi
	dec ecx
	jmp copy_loader
copy_loader_End:
    ; jmp to the new loader to continue
    jmp LOADER_REAL + LOADER_RESUME - LOADER_OFFSET

LOADER_RESUME:
	mov ecx, KERNEL_SIZE
	mov esi, KERNEL_OFFSET
	mov edi, KERNEL_REAL
copy:
	cmp ecx, 0
	je copyEnd

	mov al, byte [es:esi]; buffer address
	mov byte [ds:edi], al

	inc edi
	inc esi
	dec ecx
	jmp copy
copyEnd:
	mov	ebp, LOADER_REAL				; set the stack
	mov	esp, ebp

	call KERNEL_REAL - (LOADER_REAL - LOADER_OFFSET)		; now jump to the address of our loaded

	jmp $						; Hang.

%include "print_string_pm.asm"
%include "gdt.asm"

; global variables
LOADER_MSG	db	"####Loading Kernel#####", 0

; bootsector padding
times	510 - ( $ - $$)	db	0
dw	0xaa55