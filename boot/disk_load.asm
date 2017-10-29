; load dh sectors to ES:BX from drive dl
; load starts from cl sectors
disk_load:
	push dx				; store dx on stack so later we can recall
						; how many sectors we request to be read,
						; even if it is altered in the meantime
						
	mov	ah, 0x02		; BIOS read sector function
	mov	al, dh			; read dh sectors
	mov	ch, 0x00		; select cylinder 0
	mov	dh, 0x00		; select head 0
	int	0x13			; BIOS interrupt
	
	jc disk_error		; jump if error (i.e. carry flag set)
	
	pop dx				; read dx from the stack
	cmp	dh, al			; if al (sectors read) != dh (sectors expected)
	jne	disk_error		; display error message
	
	ret
	
disk_error:
	mov	si, DISK_ERROR_MSG
	call print_string
	jmp $
	
; variables
DISK_ERROR_MSG:
	db	"Disk error!", 0