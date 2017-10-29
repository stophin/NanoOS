;=================================
;print_string(char *addr)
;in: si(addr)
;pt: null
;=================================
print_string:
	push	si
	push	ax
.loop:
	mov	al,[si]
	inc	si
	or	al,al
	jz	.end
	mov	ah,0Eh
	mov	bx,0000Eh
	int	10h
	jmp	.loop
.end:
	pop	ax
	pop	si
	ret
;=================================