#ifndef FLOPPY_H_INCLUDED
#define FLOPPY_H_INCLUDED
#include <types.h>
#include <block.h>
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/include/floppy.h
 *		Purpose:	Header for the floppy controller
 */
 
/* Public API */
byte 		floppy_write(block_t, addr_t);
block_t	floppy_read(addr_t);
 
#endif