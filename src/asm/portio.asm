;/*
; *	Avian Kernel - Bryan Webb
; *	File:		portio.asm
; *	Purpose:	CPU bus port input and output
; */

bits 32

section .text
	global	inportb
	global	outportb

	inportb:							;Read one byte from port
		mov	edx, [esp + 4]
		in 	al, dx				; al is the lower 8 bits of eax
		ret							; dx is the lower 16 bits of edx

	outportb:						; void outportb(port_t, byte)
		mov   edx, [esp + 4]    ; 16-bit port address
		mov   al, [esp + 4 + 4] ; 8-bit data value
		out   dx, al  
		ret

; END .text