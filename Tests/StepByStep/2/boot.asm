; a boot sector that loads a loader and kernel in 32-bit protected mode
[org  0x7c00]
LOADER_BASE        equ    0x800        ; this is the memory base to which we will load our loader
                                ; loader is 512(0x200) ahead of kernel, and do not over 0xFFFF
KERNEL_BASE        equ    0x900        ; this is the memory base to which we will load our kernel
                                ; (KERNEL_BASE * 16 + OFFSET)
    mov    [BOOT_DRIVE], dl        ; BIOS stores our boot driver in dl, so it's best to remember
                                ; this for later
    mov    ax, cs
    mov    ds, ax
    mov    ss, ax
    mov    es, ax
    mov    fs, ax
    mov    gs, ax
    
    mov    bp, 0x9000                ; set the stack
    mov    sp, bp
    
    mov    si, MSG_REAL_MODE        ; announce that we are starting
    call print_string            ; booting from 16-bit real mode
    
    call load_loader            ; load our loader

    call load_kernel            ; load our kernel
    
    ;call vga_start                ; start VGA mode
    
    call switch_to_pm            ;note that we never return from here
    
    jmp $
    
; include our useful, hard-earned routine
%include "print_string.asm"
%include "disk_load.asm"
%include "gdt.asm"
%include "print_string_pm.asm"
%include "switch_to_pm.asm"

[bits 16]
; load loader
load_loader:
    mov    si, MSG_LOAD_LOADER        ; print a message to say we are loading the loader
    call print_string
    
    mov    bx, LOADER_BASE<<4        ; set up parameters for our disk_load routine, so
    mov    dh, 1                    ; that we load the first 1 sectors (excluding 
    mov    cl, 0x02                ; start reading from second sector (i.e.after the boot sector)
    mov    dl, [BOOT_DRIVE]        ; the boot sector) from the boot disk (i.e our
    call disk_load                ; loader code) to the right address
    
    ret

; load kernel
load_kernel:

    mov    si, MSG_LOAD_KERNEL        ; print a message to say we are loading the kernel
    call print_string

    mov ax, BUFFER_ADDR
    mov es, ax
    mov ax, 0                 ; ES:BX buffer

    mov ax, 0
    mov di, ax
    mov si, ax

    mov ch, 0                 ; CH cylind
    mov dh, 0                 ; DH head
    mov cl, 2                 ; CL sector, kernel starts from the 3rd sector

readFloppy:
    cmp byte [LOAD_CYLINDERS], 0
    je fin

    mov bx, 0
    inc cl
    mov ah, 0x02         ; AH read floppy
    mov al, 1             ; AL sectors
    mov dl, 0             ; DL driver

    int 0x13            ; load 1 sector into buffer
    jc error
    mov si, MSG_DOT
    call print_string

; copy from buffer to real address
copySector:
    push si
    push di
    push cx

    mov cx, 0x200        ; buffer size 512 byte
    mov di, 0
    mov si, 0
    mov ax, word [LOAD_SECTION]
    mov ds, ax            ; copy to

copy:
    cmp cx, 0
    je copyEnd

    mov al, byte [es:si]; buffer address
    mov byte [ds:di], al

    inc di
    inc si
    dec cx
    jmp copy

copyEnd:
    pop cx
    pop di
    pop si

    mov bx, ds
    add bx, 0x20                     ; add address base by 0x200 / 16
    mov ax, 0
    mov ds, ax                
    mov word [LOAD_SECTION], bx        ; save address base
    mov bx, 0

    ; end of copy sector

    cmp cl, 18                        ; every cylinder has 18 sectors
    jb readFloppy

    cmp dh, 0
    je changeHead                    ; read from head 1 after head 0
    mov dh, 0                         ; read from head 0
    inc ch                             ; and from the next cylinder
    mov cl, 0
    dec byte [LOAD_CYLINDERS]
    jmp readFloppy
changeHead:
    inc dh                             ; read from head 1
    mov cl, 0
    jmp readFloppy

fin:
    mov ax, 0
    mov ds, ax

    ret
error:
    add ah, 48
    mov byte [DISK_ERROR_MSG], ah
    mov    si, DISK_ERROR_MSG
    call print_string
    jmp $    
BUFFER_ADDR        equ 0x7e0
; global variables
LOAD_CYLINDERS    db    10            ; total size = 512 * cycliners * head(2) * 18 (sectors per cylinder)
LOAD_SECTION    dw    KERNEL_BASE

[bits 32]
; this is where we arrive after switching to and initialising protected mode.
BEGIN_PM:
    mov    ebx, MSG_PORT_MODE
    call print_string_pm        ; use out 32-bit print routine.
    
    call LOADER_BASE<<4            ; now jump to the address of our loaded
                                ; loader code, assume the brace position, 
                                ; and cross you finger, here we go!
    
    jmp $                        ; Hang.
    
; global variables
BOOT_DRIVE        db    0
MSG_LOAD_LOADER    db    "Loading loader", 0
MSG_LOAD_KERNEL    db    "Loading kernel", 0
MSG_REAL_MODE    db    "Real Mode", 0
MSG_PORT_MODE    db    "Protected Mode", 0
MSG_DOT            db     "."

; bootsector padding
times    510 - ( $ - $$)    db    0
dw    0xaa55