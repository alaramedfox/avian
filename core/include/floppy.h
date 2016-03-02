#ifndef FLOPPY_H_INCLUDED
#define FLOPPY_H_INCLUDED
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/include/floppy.h
 *		Purpose:	Header for the floppy controller
 */
#include <types.h>
#include <block.h>
#include <asmfunc.h>
 
/* Floppy CHS Address datatype */
typedef struct __CHS
{
	word cyl, head, sect;

} chs_t;

#define GAP3 0x1B

/* Function called by Interrupt Controller */
__attribute__((hot)) void floppy_handler(void);
 
/* Public API */
void floppy_init(void);
bool floppy_read_block(word, byte*, size_t);
bool floppy_write_block(word,byte*,size_t);




#endif