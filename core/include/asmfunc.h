#ifndef ASM_FUNC_H_INCLUDED
#define ASM_FUNC_H_INCLUDED
/*
 *		Avian Project - Bryan Webb
 *		File:		/core/include/asmfunc.h
 *		Purpose:	Header for all global assembly functions
 */
 
#include <types.h>

/* Port IO, defined in portio.asm */
extern byte inportb(port_t);
extern void outportb(port_t, byte);

/* Interrupt controllers, defined in interrupts.asm */
extern void load_idt(int32_t*);
extern void keyboard_irq(void);
extern void floppy_irq(void);

/* Exception controllers, defined in throw.asm */
extern void throw_zero_divide(void);


#endif