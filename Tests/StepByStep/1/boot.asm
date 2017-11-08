; boot.asm
; author stophin
;
; a boot sector that boots a c kernel in 32-bit protected mode
[org  0x7c00]
[bits 16]
KERNEL_OFFSET    equ    0x9000        ; this is the memory offset to which we will load our kernel

    mov    ax, cs
    mov    ds, ax
    mov    ss, ax
    mov    es, ax
    mov    fs, ax
    mov    gs, ax
    
    mov    [BOOT_DRIVE], dl        ; BIOS stores our boot driver in dl, so it's best to remember
                                ; this for later

    mov    bp, 0x9000                ; set the stack
    mov    sp, bp
    
    mov    si, MSG_REAL_MODE        ; announce that we are starting
    call print_string            ; booting from 16-bit real mode
    
    ;call vga_start                ; start VGA mode

    call load_kernel            ; load our kernel
    
    call switch_to_pm            ;note that we never return from here
    
    jmp $
    
; include our useful, hard-earned routine
%include "print_string.asm"
%include "disk_load.asm"
%include "gdt.asm"
%include "print_string_pm.asm"
%include "switch_to_pm.asm"

[bits 16]
; load kernel
load_kernel:
    mov    si, MSG_LOAD_KERNEL        ; print a message to say we are loading the kernel
    call print_string
    
    mov    bx, KERNEL_OFFSET        ; set up parameters for our disk_load routine, so
    mov    dh, 56                    ; that we load the first n sectors (excluding
    mov    dl, [BOOT_DRIVE]        ; the boot sector) from the boot disk (i.e our
    call disk_load                ; kernel code) to address KERNEL_OFFSET

    ret

[bits 32]
; this is where we arrive after switching to and initialising protected mode.
BEGIN_PM:
    mov    ebx, MSG_PROTECT_MODE
    call print_string_pm        ; use out 32-bit print routine.
    
    ;call KERNEL_OFFSET            ; now jump to the address of our loaded
                                ; kernel code, assume the brace position,
                                ; and cross you finger, here we go!
    
    jmp $                        ; Hang.
    
; global variables
BOOT_DRIVE            db    0
MSG_LOAD_KERNEL        db    "Loading kernel into memory", 0
MSG_REAL_MODE        db    "Started in 16-bit Real Mode", 0
MSG_PROTECT_MODE    db    "Successfully landed in 32-bit Protected Mode", 0

; bootsector padding
times    510 - ( $ - $$)    db    0
dw    0xaa55