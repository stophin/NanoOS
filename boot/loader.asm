; loader.asm
[org  0x10000]	; where loader will be loaded to
	jmp	short LoaderStart

%include "FAT12.inc"

BaseOfKernelFile	equ	0x00
OffsetOfKernelFile	equ	0x100000

BaseTmpOfKernelAddr	equ	0x00
OffsetTmpOfKernelFile	equ	0x7E00

MemoryStructBufferAddr	equ	0x7E00

[bits 16]
; this is where we arrive after switching to and initialising protected mode.
LoaderStart:
	mov	ax,	cs
	mov	ds,	ax
	mov	es,	ax
	mov	ax,	0x00
	mov	ss,	ax
	mov	sp,	0x7c00

;=======	open address A20
	push	ax
	in	al,	92h
	or	al,	00000010b
	out	92h,	al
	pop	ax

	cli

	lgdt	[gdt_descriptor]	

	mov	eax,	cr0
	or	eax,	1
	mov	cr0,	eax

	mov	ax,	DATA_SEG
	mov	fs,	ax
	mov	eax,	cr0
	and	al,	11111110b
	mov	cr0,	eax

	sti
	
; load kernel
load_kernel:
	;mov	si, MSG_LOAD_KERNEL		; print a message to say we are loading the kernel
	;call print_string

;=======	reset floppy

	xor	ah,	ah
	xor	dl,	dl
	int	13h

;=======	search kernel.bin
	mov	word	[SectorNo],	SectorNumOfRootDirStart

Lable_Search_In_Root_Dir_Begin:

	cmp	word	[RootDirSizeForLoop],	0
	jz	Label_No_LoaderBin
	dec	word	[RootDirSizeForLoop]	
	mov	ax,	00h
	mov	es,	ax
	mov	bx,	8000h
	mov	ax,	[SectorNo]
	mov	cl,	1
	call	Func_ReadOneSector
	mov	si,	KernelFileName
	mov	di,	8000h
	cld
	mov	dx,	10h
	
Label_Search_For_LoaderBin:

	cmp	dx,	0
	jz	Label_Goto_Next_Sector_In_Root_Dir
	dec	dx
	mov	cx,	11

Label_Cmp_FileName:

	cmp	cx,	0
	jz	Label_FileName_Found
	dec	cx
	lodsb	
	cmp	al,	byte	[es:di]
	jz	Label_Go_On
	jmp	Label_Different

Label_Go_On:
	
	inc	di
	jmp	Label_Cmp_FileName

Label_Different:

	and	di,	0FFE0h
	add	di,	20h
	mov	si,	KernelFileName
	jmp	Label_Search_For_LoaderBin

Label_Goto_Next_Sector_In_Root_Dir:
	
	add	word	[SectorNo],	1
	jmp	Lable_Search_In_Root_Dir_Begin
	
;=======	display on screen : ERROR:No KERNEL Found

Label_No_LoaderBin:

	mov	ax,	1301h
	mov	bx,	008Ch
	mov	dx,	0300h		;row 3
	mov	cx,	21
	push	ax
	mov	ax,	ds
	mov	es,	ax
	pop	ax
	mov	bp,	NoLoaderMessage
	int	10h
	jmp	$

;=======	found loader.bin name in root director struct

Label_FileName_Found:
	mov	ax,	RootDirSectors
	and	di,	0FFE0h
	add	di,	01Ah
	mov	cx,	word	[es:di]
	push	cx
	add	cx,	ax
	add	cx,	SectorBalance
	mov	eax,	BaseTmpOfKernelAddr;BaseOfKernelFile
	mov	es,	eax
	mov	bx,	OffsetTmpOfKernelFile;OffsetOfKernelFile
	mov	ax,	cx

Label_Go_On_Loading_File:
	push	ax
	push	bx
	mov	ah,	0Eh
	mov	al,	'.'
	mov	bl,	0Fh
	int	10h
	pop	bx
	pop	ax

	mov	cl,	1
	call	Func_ReadOneSector
	pop	ax

;;;;;;;;;;;;;;;;;;;;;;;	
	push	cx
	push	eax
	push	fs
	push	edi
	push	ds
	push	esi

	mov	cx,	200h
	mov	ax,	BaseOfKernelFile
	mov	fs,	ax
	mov	edi,	dword	[OffsetOfKernelFileCount]

	mov	ax,	BaseTmpOfKernelAddr
	mov	ds,	ax
	mov	esi,	OffsetTmpOfKernelFile

Label_Mov_Kernel:	;------------------
	
	mov	al,	byte	[ds:esi]
	mov	byte	[fs:edi],	al

	inc	esi
	inc	edi

	loop	Label_Mov_Kernel

	mov	eax,	0x1000
	mov	ds,	eax

	mov	dword	[OffsetOfKernelFileCount],	edi

	pop	esi
	pop	ds
	pop	edi
	pop	fs
	pop	eax
	pop	cx
;;;;;;;;;;;;;;;;;;;;;;;	

	call	Func_GetFATEntry
	cmp	ax,	0FFFh
	jz	Label_File_Loaded
	push	ax
	mov	dx,	RootDirSectors
	add	ax,	dx
	add	ax,	SectorBalance
;	add	bx,	[BPB_BytesPerSec]	

	jmp	Label_Go_On_Loading_File

Label_File_Loaded:
		
	mov	ax, 0B800h
	mov	gs, ax
	mov	ah, 0Fh				; 0000: 黑底    1111: 白字
	mov	al, 'G'
	mov	[gs:((80 * 0 + 39) * 2)], ax	; 屏幕第 0 行, 第 39 列。

KillMotor:
	
	push	dx
	mov	dx,	03F2h
	mov	al,	0	
	out	dx,	al
	pop	dx

VGA_START:
	push ds
	push ax

	mov	ax,	0x9000
	mov	ds,	ax

	call vga_start				; start VGA mode
	;call vga_start_hd			; start HD-VGA mode
	;call vga_start_vbe			; start VBE mode

	pop ax
	pop ds

;=======	init IDT GDT goto protect mode 

	cli			;======close interrupt

	lgdt	[gdt_descriptor]

;	lidt	[IDT_POINTER]

	mov	eax,	cr0
	or	eax,	1
	mov	cr0,	eax	

	jmp	dword CODE_SEG:GO_TO_TMP_Protect

[bits 32]
GO_TO_TMP_Protect:

	mov	ax,	0x10
	mov	ds,	ax
	mov	es,	ax
	mov	fs,	ax
	mov	ss,	ax
	mov	esp,	7E00h
	
;=======	load GDTR
	
	lgdt	[gdt_descriptor]
	mov	ax,	0x10
	mov	ds,	ax
	mov	es,	ax
	mov	fs,	ax
	mov	gs,	ax
	mov	ss,	ax

	mov	esp,	0x10000
	
	;jmp SKIP_PAGE
;----------------------------------------------------------------------------
; 分页机制使用的常量说明
;----------------------------------------------------------------------------
PG_P		EQU	1	; 页存在属性位
PG_RWR		EQU	0	; R/W 属性位值, 读/执行
PG_RWW		EQU	2	; R/W 属性位值, 读/写/执行
PG_USS		EQU	0	; U/S 属性位值, 系统级
PG_USU		EQU	4	; U/S 属性位值, 用户级

PageDirBase		equ	300000h	; 页目录开始地址:	2M
PageTblBase		equ	301000h	; 页表开始地址:		2M + 4K

; 为简化处理, 所有线性地址对应相等的物理地址. 并且不考虑内存空洞.
Setup_Paging:
	mov ax, DATA_SEG
	mov es, ax
	mov edi, PageDirBase  	; PDE: DATA_SEG:0
	xor eax, eax
	mov eax, PageTblBase | PG_P | PG_USS | PG_RWW

	mov ecx, 1024	; 1024 PDE
LOOP_PDE:
	stosd			; mov [es:di], eax; edi = edi + 4
	add eax, 4096	
	loop LOOP_PDE	; loop until ecx - 1 == 0

	
	mov eax, 1024	; 1024 PDE
	mov ebx, 1024	; 1024 PTE per PDE
	mul ebx
	mov ecx, eax	; 1024 * 1024 PTE
	mov ax, DATA_SEG
	mov es, ax
	mov edi, PageTblBase
	xor eax, eax
	mov eax, PG_P | PG_USS | PG_RWW

LOOP_PTE:
	stosd
	add eax, 4096
	loop LOOP_PTE

;=======	load	cr3

	mov	eax,	PageDirBase
	mov	cr3,	eax

;=======	open PE and paging

	mov	eax,	cr0
	bts	eax,	0
	bts	eax,	31
	mov	cr0,	eax
	
SKIP_PAGE:
	;jmp	CODE_SEG:OffsetOfKernelFile
	push	CODE_SEG
	push OffsetOfKernelFile
	retf

; include our useful, hard-earned routine
%include "print_string.asm"
%include "gdt.asm"
%include "vga_start.asm"
%include "disk_load.asm"

;=======	tmp variable

RootDirSizeForLoop	dw	RootDirSectors
SectorNo		dw	0
Odd			db	0
OffsetOfKernelFileCount	dd	OffsetOfKernelFile

; global variables
MSG_LOAD_KERNEL	db	"Loading kernel", 0
NoLoaderMessage:	db	"ERROR:No KERNEL Found"
KernelFileName:		db	"KERNEL  BIN",0
