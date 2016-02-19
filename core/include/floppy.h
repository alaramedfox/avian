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

/* Function called by Interrupt Controller */
void floppy_handler(void);
 
/* Public API */
status_t	floppy_init(void);
status_t floppy_write(block_t, addr_t);
block_t	floppy_read(addr_t);

/* Static helper functions */

static void 	__floppy_irq_acknowledge(void);
static void 	__floppy_reset(void);
static chs_t	__lba_convert(dword);


#endif