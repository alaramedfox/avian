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
	   push es
	   push eax
		call catch_exception
		jmp [eax]
		iret
		
   throw_zero_divide:
      cli
      mov eax, [esp]
      push eax
      call catch_zero_divide
      iret
      
   global throw_irq_0
   throw_irq_0:
      mov eax, [esp]
      push 0
      push eax
      call catch_exception
      iret
      
   global throw_irq_1
   throw_irq_1:
      mov eax, [esp]
      push 1
      push eax
      call catch_exception
      iret
      
   global throw_irq_2
   throw_irq_2:
      mov eax, [esp]
      push 2
      push eax
      call catch_exception
      iret
      
   global throw_irq_3
   throw_irq_3:
      mov eax, [esp]
      push 3
      push eax
      call catch_exception
      iret
      
   global throw_irq_4
   throw_irq_4:
      mov eax, [esp]
      push 4
      push eax
      call catch_exception
      iret
      
   global throw_irq_5
   throw_irq_5:
      mov eax, [esp]
      push 5
      push eax
      call catch_exception
      iret
      
   global throw_irq_6
   throw_irq_6:
      mov eax, [esp]
      push 6
      push eax
      call catch_exception
      iret
      
   global throw_irq_7
   throw_irq_7:
      mov eax, [esp]
      push 7
      push eax
      call catch_exception
      iret
      
   global throw_irq_8
   throw_irq_8:
      pop eax ; Ignore error code
      mov eax, [esp]
      push 8
      push eax
      call catch_exception
      iret
      
   global throw_irq_9
   throw_irq_9:
      mov eax, [esp]
      push 9
      push eax
      call catch_exception
      iret
      
   global throw_irq_10
   throw_irq_10:
      mov eax, [esp]
      push 10
      push eax
      call catch_exception
      iret
      
   global throw_irq_11
   throw_irq_11:
      mov eax, [esp]
      push 11
      push eax
      call catch_exception
      iret
      
   global throw_irq_12
   throw_irq_12:
      mov eax, [esp]
      push 12
      push eax
      call catch_exception
      iret
      
   global throw_irq_13
   throw_irq_13:
      mov eax, [esp]
      push 13
      push eax
      call catch_exception
      iret
      
   global throw_irq_14
   throw_irq_14:
      mov eax, [esp]
      push 14
      push eax
      call catch_exception
      iret
      
   global throw_irq_15
   throw_irq_15:
      mov eax, [esp]
      push 15
      push eax
      call catch_exception
      iret
      
   global throw_irq_16
   throw_irq_16:
      mov eax, [esp]
      push 16
      push eax
      call catch_exception
      iret
      
   global throw_irq_17
   throw_irq_17:
      mov eax, [esp]
      push 17
      push eax
      call catch_exception
      iret
      
   global throw_irq_18
   throw_irq_18:
      mov eax, [esp]
      push 18
      push eax
      call catch_exception
      iret
      
	
; END .text