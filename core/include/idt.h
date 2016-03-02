#ifndef IDT_H_INCLUDED
#define IDT_H_INCLUDED
/*
 *		Avian Kernel - Bryan Webb
 *	  	File:		/core/include/idt.h
 *	  	Purpose: Header file for idt.c
 */
 
#include <types.h>
#include <asmfunc.h>

enum __IDT_DEFS
{
	IDT_SIZE = 256,
	KERNEL_OFFSET = 0x08,
};

typedef enum __IDT_GATES
{
	INTERRUPT_GATE = 0x8E,
	
	
} type_attr_t;

void idt_add_handler(addr_t, byte);
void idt_add_exception(addr_t, byte);

/**** Static Helper Functions ******/
static void idt_add_interrupt(addr_t, byte);
static void idt_write_table(void);
 
#endif