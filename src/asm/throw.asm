;/*
; *	Avian Kernel - Bryan Webb
; *	File:		throw.asm
; *	Purpose:	Catch CPU exceptions and throw correct error
; */
bits 32							; NASM directive - 32 bit

section .text
	global throw_exception
	extern catch_exception
	
	throw_exception:
	   ;call iptr
	   cli
	   mov eax, [esp]
	   push gs
	   push eax
		call catch_exception
		iret
	
; END .text