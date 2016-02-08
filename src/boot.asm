;;kernel.asm
bits 32							;nasm directive - 32 bit
section .text
	align 4						;multiboot spec
	dd 	0x1BADB002        ;magic
	dd 	0x00              ;flags
	dd - (0x1BADB002 + 0x00);checksum. m+f+c should be zero

global 	start					;kernel entry point
global	keyboard_handler	;Other keyboard driver (?)
global 	__load_idt			;hardware interrupts
global 	__read_port			;read hardware data
global 	__write_port		;write hardware data
global	__i_listen			;Start listening for interrupts
global	__i_ignore			;Quit listening for interrupts
global	__shut_down			;Shut down the system

extern 	main	       		;kmain is the C entry point
extern	keyboard_driver	;Keyboard driver

start:
	cli 							;block interrupts
	mov 	esp, stack_space	;set stack pointer
	call	main					;Invoke the C kernel
	hlt		 					;halt the CPU
	
__i_listen:
	sti							;Enable interrupts
	ret
	
__i_ignore:
	cli							;Disable interrupts
	ret
	
__load_idt:
	mov	edx, [esp + 4]
	lidt [edx]
	sti
	ret
	
__read_port:				;Port data IN
	mov	edx, [esp + 4]
						;al is the lower 8 bits of eax
	in 	al, dx			;dx is the lower 16 bits of edx
	ret

__write_port:				;Port data OUT
	mov   edx, [esp + 4]    
	mov   al, [esp + 4 + 4]  
	out   dx, al  
	ret
; end __write_port

__shut_down:
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
	mov     cx, 0003
	int     0x15
	
	ret
	
keyboard_handler:                 
	call	 keyboard_driver	;call the keyboard driver
	iretd

section .bss
	resb 8192				;8KB for stack
	
stack_space:
