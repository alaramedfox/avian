#ifndef IDT_H_INCLUDED
#define IDT_H_INCLUDED
// ======================================================================== */
//		Avian Kernel   Bryan Webb (C) 2016
//	  	File:		      avian/include/idt.h
//	  	Purpose:       Header for the Interrupt Descriptor Table
// ======================================================================== */
 
#include <stdlib.h>
#include <asmfunc.h>

enum __IDT_DEFS
{
	IDT_SIZE = 256,
	KERNEL_OFFSET = 0x08,
};

enum __IDT_GATES
{
	INTERRUPT_GATE = 0x8E,
	
	
};

void idt_add_handler(addr_t, byte);
void idt_add_exception(addr_t, byte);
 
#endif