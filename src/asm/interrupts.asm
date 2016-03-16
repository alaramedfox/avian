;/*
; *	Avian Kernel - Bryan Webb
; *	File:		interrupts.asm
; *	Purpose:	Wrapper functions for interrupt routines
; */

bits 32	; NASM directive for 32-bit mode

section .text
	global	load_idt
	
	global	keyboard_irq
	extern	keyboard_handler
	
	global	floppy_irq
	extern	floppy_handler
	
	global	pit_irq
	extern	pit_handler

	load_idt:
	   cli
		mov	edx, [esp + 4]
		lidt [edx]
		sti	;<-- THIS CRASHES THE KERNEL
		ret
	
	keyboard_irq:
	   ;cli          
		call	 keyboard_handler
		;sti
		iretd
		
	floppy_irq:
	   ;cli
		call	floppy_handler
		;sti
		iretd
		
	;pit_irq:
	;   ;cli
	;	call	pit_handler
	;	iretd

; END .text