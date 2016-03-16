;/*
; *	Avian Kernel - Bryan Webb
; *	File:		boot.asm
; *	Purpose:	Entry point from Bootloader
; */
bits 32							; NASM directive - 32 bit

section .mbh
	align 4						; Multiboot spec
	dd 	0x1BADB002        ; Magic
	dd 	0x00              ; Flags
	dd - (0x1BADB002+0x00)	; Checksum - m+f+c should be zero
; END .mbh

section .text

   global 	start				; Bootloader entry point
	extern 	avian_main	   ; Kernel entry point

	start:
		cli 						; Block interrupts
		mov 	esp, __stack	; Set stack pointer
		call	avian_main		; Invoke the C kernel
		hlt		 				; Halt the CPU
; END .text
	
section .bss
	resb 8192					; 8 KiB for stack
	
	__stack:

; END .bss