#ifndef IDT_H_INCLUDED
#define IDT_H_INCLUDED
/*
 *	Popcorn Kernel
 *	  File:		/include/idt.h
 *	  Purpose: 	Header file for idt.c
 */
 
#include <types.h>
#include <asmfunc.h>
 
#define IDT_SIZE 	256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET	0x08;

struct IDT_entry;
void idt_init(void);
 
#endif