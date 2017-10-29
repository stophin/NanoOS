; kernel_entry.asm
; author stophin
;
; ensure that we jump straight into the kernel's entry function
[bits 32]					; we're in protected mode by now, so use 32-bit instructions
[extern main]				; declate that we will be referencing the external symbol main
							; so the linke can substitute the final address
	mov byte [0xb8000], 102
	call main				; invoke main() in our c kernel
	jmp $					; hang forever when we return from the kernel