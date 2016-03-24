;/*
; *	Avian Kernel - Bryan Webb
; *	File:		throw.asm
; *	Purpose:	Catch CPU exceptions and throw correct error
; */
bits 32							; NASM directive - 32 bit

section .text
	global throw_exception
	extern catch_exception
	
	global throw_zero_divide
	extern catch_zero_divide
	
	throw_exception:
	   ;call iptr
	   cli
	   mov eax, [esp]
	   push 255
	   push eax
		call catch_exception
		iret
		
   throw_zero_divide:
      cli
      mov eax, [esp]
      push eax
      call catch_zero_divide
      iret
	
; END .text