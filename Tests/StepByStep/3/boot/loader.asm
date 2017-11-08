; loader.asm
[org  0x8000]   ; where loader will be loaded to
KERNEL_OFFSET   equ 0x9000      ; this is the memory offset to which we have loaded our kernel
KERNEL_REAL     equ 0x10000     ; this is the real memory addres to which we started our kernel
LOAD_CYLINDERS  equ 10
                                ; total size = 512 * cycliners * head(2) * 18 (sectors per cylinder)
TOTAL_SIZE      equ 512 * LOAD_CYLINDERS * 2 * 18   
[bits 32]
; this is where we arrive after switching to and initialising protected mode.
BEGIN_LOADER:
    mov ebx, LOADER_MSG
    call print_string_pm        ; use out 32-bit print routine.
    
    ; move kernel to KERNEL_REAL
    ; in protect mode, you can use over 1M address
    ; (we do not need to move at this time)
    ;mov cx, TOTAL_SIZE
    ;mov esi, KERNEL_OFFSET
    ;mov edi, KERNEL_REAL
    ;cld                        ; increase
    ;rep movsb

    call KERNEL_OFFSET          ; now jump to the address of our loaded
    
    jmp $                       ; Hang.

%include "print_string_pm.asm"

; global variables
LOADER_MSG  db  "####Loading Kernel#####", 0

; bootsector padding
times   510 - ( $ - $$) db  0
dw  0xaa55