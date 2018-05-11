; loader.asm
[org  0x8000]	; where loader will be loaded to
KERNEL_OFFSET   equ 0x9000  ; this is the memory offset to which we have loaded our kernel
                                ; minimum memory required 1024M
KERNEL_REAL     equ 0x30000000  ; this is the real memory addres to which we started our kernel
LOAD_CYLINDERS  equ 10      ; total size = 512 * cycliners * head(2) * 18 (sectors per cylinder)
KERNEL_SIZE     equ 512 * LOAD_CYLINDERS * 2 * 18

LOADER_OFFSET equ 0x8000    ; this is the memory offset to which we have loaded our loader
LOADER_REAL   equ 0x90000   ; this is the memory address to which we will load out load
LOADER_SIZE   equ 512       ; loader size = 512
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
	je copy_loader_end

	mov al, byte [es:esi]; buffer address
	mov byte [ds:edi], al

	inc edi
	inc esi
	dec ecx
	jmp copy_loader
copy_loader_end:
    ; jmp to the new loader to continue
    jmp LOADER_REAL + LOADER_RESUME - LOADER_OFFSET

LOADER_RESUME:
	mov ecx, KERNEL_SIZE
	mov esi, KERNEL_OFFSET
	mov edi, KERNEL_REAL
copy:
	cmp ecx, 0
	je copy_end

	mov al, byte [es:esi]; buffer address
	mov byte [ds:edi], al

	inc edi
	inc esi
	dec ecx
	jmp copy
copy_end:
	mov	ebp, KERNEL_REAL				; set the stack
	mov	esp, ebp

	call KERNEL_REAL - (LOADER_REAL - LOADER_OFFSET)	; now jump to the address of our relocated kernel

	jmp $						; Hang.

%include "print_string_pm.asm"
%include "gdt.asm"

; global variables
LOADER_MSG	db	"xProtected Mode####Loading Kernel#####", 0

; loader padding
; loader cannot exceed 512
times	510 - ( $ - $$)	db	0
dw	0xaa55