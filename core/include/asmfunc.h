#ifndef ASM_FUNC_H_INCLUDED
#define ASM_FUNC_H_INCLUDED
/*
 *		Avian Project - Bryan Webb
 *		File:		/core/include/asmfunc.h
 *		Purpose:	Header for all global assembly functions
 */
 
#include <types.h>

/* Port IO */
extern byte inportb(port_t);
extern void outportb(port_t, byte);

extern void ASM_load_idt(int32_t*);
extern void ASM_kb_driver(void);
extern void ASM_shut_down(void);


#endif