#ifndef ASM_FUNC_H_INCLUDED
#define ASM_FUNC_H_INCLUDED
/*
 *		Popcorn Kernel - Bryan Webb
 *		File:		/include/asmfunc.h
 *		Purpose:	Header for all global assembly functions
 */
 
#include <types.h>

extern byte ASM_read_port(word);
extern void ASM_write_port(word, byte);
extern void ASM_load_idt(int32_t*);
extern void ASM_kb_driver(void);


#endif