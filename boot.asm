;;kernel.asm
bits 32							;nasm directive - 32 bit
section .text
	align 4						;multiboot spec
	dd 	0x1BADB002        ;magic
	dd 	0x00              ;flags
	dd - (0x1BADB002+0x00)	;checksum. m+f+c should be zero

global 	start					;kernel entry point
global	ASM_kb_driver		;Other keyboard driver (?)
global 	ASM_load_idt		;hardware interrupts
global 	ASM_inb				;read hardware data
global 	ASM_outb				;write hardware data
global	ASM_shut_down		;Shut down the system

extern 	C_main	       	;main is the C entry point
extern	C_kb_driver			;Keyboard driver

start:
	cli 							;block interrupts
	mov 	esp, stack_space	;set stack pointer
	call	C_main					;Invoke the C kernel
	hlt		 					;halt the CPU
	
ASM_load_idt:
	mov	edx, [esp + 4]
	lidt [edx]
	sti
	ret
	
ASM_inb:							;Port data IN
	mov	edx, [esp + 4]
									;al is the lower 8 bits of eax
	in 	al, dx				;dx is the lower 16 bits of edx
	ret

ASM_outb:						;Port data OUT
	mov   edx, [esp + 4]    
	mov   al, [esp + 4 + 4]  
	out   dx, al  
	ret
; end ASM_outb

ASM_shut_down:
	;Connect to APM API
	mov     ax,	0x5301
	xor     bx, bx
	int     0x15

	;Try to set APM version (to 1.2)
	mov     ax,	0x530E
	xor     bx,	bx
	mov     cx,	0x0102
	int     0x15

	;Turn off the system
	mov     ax, 0x5307
	mov     bx, 0x0001
	mov     cx, 0x0003
	int     0x15
	
	ret
	
ASM_kb_driver:                 
	call	 C_kb_driver	;call the keyboard driver
	iretd

section .bss
	resb 16384				;16KB for stack
	
stack_space:
