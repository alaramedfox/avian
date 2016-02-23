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

#define DMA_BUFFER_ADDRESS  0x80000

/* Function called by Interrupt Controller */
void floppy_handler(void);
 
/* Public API */
void	floppy_init(void);

/* Static helper functions */
static bool floppy_data_transfer(int,block_t*,bool,size_t);
static void 	floppy_reset(void);
static bool 	floppy_command_wait(bool);
static void		floppy_start_motor(int);
static void		floppy_stop_motor(int);
static int 		floppy_send_byte(byte);
static byte		floppy_read_byte(void);
static chs_t	lba_convert(int);


#endif