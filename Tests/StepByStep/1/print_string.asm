; print_strin.asm
; author stophin
;
[bits 16]
; print_string(SI)
print_string:
    mov ax, [si]
    mov bp, ax
    mov cx, 36
    mov ax, 01301h
    mov bx, 000ch
    mov dl, 0
    int 10h
    ret