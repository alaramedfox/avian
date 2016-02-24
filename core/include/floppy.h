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

#define DMA_BUFFER_ADDRESS  0x1000
#define GAP3 0x1B

/* Function called by Interrupt Controller */
void floppy_handler(void);
 
/* Public API */
void floppy_init(void);
bool floppy_read_block(int, byte*, size_t);
bool floppy_write_block(int,byte*,size_t);

/* Static helper functions */
static void 	floppy_configure(void);
static void 	floppy_specify(byte,byte,byte);
static bool 	floppy_data_transfer(int,byte*,bool,size_t);
static int		floppy_seek(byte);
static void		floppy_recalibrate(void);
static void 	floppy_reset(void);
static bool 	floppy_command_wait(bool);
static void		floppy_start_motor(int);
static void		floppy_stop_motor(int);
static int 		floppy_send_byte(byte);
static byte		floppy_read_byte(void);
static chs_t	lba_convert(int);


#endif