; system_lib.asm
; author stophin
;
; System lib functions, will be invoked by C kernel
[bits 32]
global	disp_pos, disp_base
global	disp_str_asm, disp_str_col_asm

[section .data]
disp_base	dd	0xb8000
disp_pos	dd 	0xb8000
[section .text]
; void disp_str_asm(char *)
disp_str_asm:
	push ebp
	mov	ebp, esp

	mov esi, [ebp + 8]
	mov edi, [disp_pos]
	mov	ah, 0Fh
.1:
	lodsb
	test al, al
	jz	.2
	cmp al, 0Ah		; return?
	jnz .3
	push eax
	mov eax, edi
	sub eax, [disp_base]
	mov	bl, 160		; 80 * 2
	div bl
	and eax, 0FFh
	inc eax
	mov bl, 160
	mul bl
	add eax, [disp_base]
	mov edi, eax
	pop eax
	jmp .1
.3:
	mov [edi], ax
	add edi, 2
	jmp .1
	
.2:
	mov [disp_pos], edi
	
	mov esp, ebp
	pop ebp

	ret


; void disp_str_col_asm(char *, char)
disp_str_col_asm:
	push ebp
	mov	ebp, esp

	mov esi, [ebp + 8]
	mov edi, [disp_pos]
	mov	ah, [ebp + 12]		; color
.1:
	lodsb
	test al, al
	jz	.2
	cmp al, 0Ah		; return?
	jnz .3
	push eax
	mov eax, edi
	sub eax, [disp_base]
	mov	bl, 160		; 80 * 2
	div bl
	and eax, 0FFh
	inc eax
	mov bl, 160
	mul bl
	add eax, [disp_base]
	mov edi, eax
	pop eax
	jmp .1
.3:
	mov [edi], ax
	add edi, 2
	jmp .1

.2:
	mov [disp_pos], edi
	
	mov esp, ebp
	pop ebp

	ret

