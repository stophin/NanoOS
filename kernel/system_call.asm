; system_call.asm
; author stophin
;
; System call functions, will be invoked by C kernel
[bits 32]
; This file's object will link after kenel.o so it's not
; necessary to declare in system_call.h, while declare in
; global here is mandatory
global	io_hlt, io_cli, io_sti, io_stihlt, io_nop
global	io_store_eflags, io_load_eflags, store_cr0, load_cr0
global	load_gdtr, load_idtr, load_ldtr, load_enable
global	task_switch3, task_switch4, far_jump
global	write_mem, check_mem
global	int_handler21_asm, int_handler2c_asm, int_handler20_asm

; void io_hlt(void)
io_hlt:
	hlt
	ret
	
; void io_cli(void)
io_cli:
	cli
	ret
	
; void io_sti(void)
io_sti:
	sti
	ret
	
; void io_stihlt(void)
io_stihlt:
	sti
	hlt
	ret

; void io_nop(void)
io_nop:
	nop
	ret

; void io_store_eflags(int)
io_store_eflags:
	mov	eax, [esp + 0x04]
	push eax
	popfd
	ret
	
; int io_load_eflags(void)
io_load_eflags:
	pushfd
	pop	eax
	ret

; void store_cr0(int)
store_cr0:
	mov	eax, [esp + 4]
	mov	cr0, eax
	ret

; void load_cr0(void)
load_cr0:
	mov	eax, cr0
	ret
	
; void load_gdtr(int limit, int address)
load_gdtr:
	mov	ax, [esp + 4]
	mov	[esp + 6], ax
	lgdt [esp + 6]		; [esp + 6] <- limit, [esp + 8] <- address, total 48byte
	ret
	
; void load_idtr(int limit, int address)
load_idtr:
	mov	ax, [esp + 4]
	mov	[esp + 6], ax
	lidt [esp + 6]
	ret

; void load_ldtr(int ltr)
load_ldtr:
	ltr [esp + 4]
	ret
	
load_enable:
	jmp	0x08:csinit
	ret
csinit:	
	mov byte [0xb8000], 'O'
	ret

; void task_switch4()
task_switch4:
	jmp 4 * 8 : 0
	ret

; void task_switch3()
task_switch3:
	jmp 3 * 8 : 0
	ret

; void far_jump(int eip, int cs)
far_jump:
	jmp far [esp + 4]	; eip, cs
	ret
; If we do need to use local variables 
; in function or call other sub functions, 
; then we need to push ebp, and mov ebp, esp
; and finally mov esp, ebp, and pop ebp,
; use [ebp + 0x08 + 0x04 * (n - 1)] as the n'th parameter.
; If we needn't any local variables or call any
; other sub functions then these process is not 
; necessary(Well, adding them does not hurt anything),
; and use [esp + 0x04 * n] as the n'th parameter.

; void write_mem(int addr, int data)
write_mem:
	mov	ecx, [esp + 4]
	mov	al, [esp + 8]
	mov [ecx], al
	ret

; int check_mem(int start, int end)
check_mem:
	push edi
	push esi
	push ebx
	mov	esi, 0xaa55aa55			; pat0
	mov	edi, 0x55aa55aa			; pat1
	mov	eax, [esp + 12 + 4]		; i = start, 
								; pushed 3 registers so 3 * 4
.loop:
	mov	ebx, eax
	add ebx, 0xffc				; p = i + 0xffc
	mov	edx, [ebx]				; old = *p
	mov	[ebx], esi				; *p = pat0
	xor	dword [ebx], 0xffffffff	; *p ^= 0xffffffff
	cmp	edi, [ebx]				; if (*p != pat1) goto fin
	jne	.fin
	xor	dword [ebx], 0xffffffff	; *p ^= 0xffffffff
	cmp	esi, [ebx]				; if (*p != pat0) goto fin
	jne .fin
	mov	[ebx], edx				; *p = old
	add eax, 0x1000				; i += 0x1000
	cmp	eax, [esp + 12 + 8]		; if (i <= end) goto loop
	jbe	.loop

.fin:
	mov	[ebx], edx
	pop ebx
	pop esi
	pop edi
	ret

	
[extern int_handler21]
; void int_handler21_asm()
int_handler21_asm:
	push ebp
	mov	ebp, esp
	
	call int_handler21
	
	leave
	iretd
[extern int_handler2c]
; void int_handler2c_asm()
int_handler2c_asm:
	push ebp
	mov	ebp, esp
	
	call int_handler2c
	
	leave
	iretd
[extern int_handler20]
; void int_handler20_asm()
int_handler20_asm:
	push ebp
	mov	ebp, esp

	push es
	push ds

	mov eax, esp
	push eax
	mov ax, ss
	mov ds, ax
	mov es, ax
	
	call int_handler20

	pop eax
	
	pop ds
	pop es

	leave
	iretd