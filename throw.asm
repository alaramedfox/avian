;/*
; *	Avian Kernel - Bryan Webb
; *	File:		throw.asm
; *	Purpose:	Catch CPU exceptions and throw correct error
; */
bits 32							; NASM directive - 32 bit

section .text
	global throw_zero_divide
	extern catch_zero_divide
	
	global throw_exception
	extern catch_exception
	
	throw_exception:
		call catch_exception
		iret
	
	throw_zero_divide:
		call catch_zero_divide
		iret
	
; END .text