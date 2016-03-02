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
	
	global throw_double_fault
	extern catch_double_fault
	
	throw_exception:
		call catch_exception
		iret
	
	throw_zero_divide:
		call catch_zero_divide
		iret
		
	throw_double_fault:
		call catch_double_fault
		iret
	
; END .text