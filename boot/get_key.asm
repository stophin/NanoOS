;=================================
;get_key
;in: null
;pt: null
;=================================
get_key:
	mov	ah,0x00
	int	0x16			; get the next char using 16h interrupt
	ret
;=================================