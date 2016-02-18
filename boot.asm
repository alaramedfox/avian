;;kernel.asm
bits 32							;nasm directive - 32 bit
section .text
	align 4						;multiboot spec
	dd 	0x1BADB002        ;magic
	dd 	0x00              ;flags
	dd - (0x1BADB002+0x00)	;checksum. m+f+c should be zero

global 	start					;kernel entry point


; ===[ Interrupt Service Routines ]================== ;
global 	keyboard_irq		;ASM wrapper for kb driver
extern	keyboard_driver	;C function for keyboard driver
; =================================================== ;

; ===[ External C functions ]======================== ;
extern 	avian_main	      ;C Source entry point
; =================================================== ;

; ===[ Functions used in C source ]================== ;
global	load_idt				;Load the Interrupt Descriptor Table
global 	inportb				;read hardware data
global 	outportb				;write hardware data
; =================================================== ;

start:
	cli 							;block interrupts
	mov 	esp, stack_space	;set stack pointer
	call	avian_main			;Invoke the C kernel
	hlt		 					;halt the CPU
	
load_idt:
	mov	edx, [esp + 4]
	lidt [edx]
	sti
	ret
	
inportb:							;Port data IN
	mov	edx, [esp + 4]
	in 	al, dx				;al is the lower 8 bits of eax
	ret							;dx is the lower 16 bits of edx

outportb:						;Port data OUT
	mov   edx, [esp + 4]    
	mov   al, [esp + 4 + 4]  
	out   dx, al  
	ret
	
keyboard_irq:                 
	call	 keyboard_driver	;Call the keyboard driver
	iretd

section .bss
	resb 8192					;18KB for stack
	
stack_space:
