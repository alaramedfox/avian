#ifndef FLOPPY_H_INCLUDED
#define FLOPPY_H_INCLUDED
#include <types.h>
#include <block.h>
#include <asmfunc.h>
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/include/floppy.h
 *		Purpose:	Header for the floppy controller
 */
 
typedef struct __DOR_BITFLAGS
{
	byte MOTD: 1; byte MOTC: 1; byte MOTB: 1; byte MOTA: 1;
	byte IRQ: 1; byte RESET: 1; byte DSEL: 2;
	
} dor_bitflag_t;

typedef struct __MSR_BITFLAGS
{
	byte RQM: 1; byte DIO: 1; byte NDMA: 1; byte CB: 1;
	byte ACTD: 1; byte ACTC: 1; byte ACTB: 1; byte ACTA: 1;
	
} msr_bitflag_t;
 
/* Floppy CHS Address datatype */
typedef struct __CHS
{
	word cyl, head, sect;

} chs_t;
 
/* Public API */
void		floppy_init(void);
byte 		floppy_write(block_t, addr_t);
block_t	floppy_read(addr_t);

/* Static helper functions */
static chs_t lba_convert(dword);
static void motor_power(byte,bool);
static void issue_command(byte, byte*, byte*);
static bool issue_command_args(byte*);
static msr_bitflag_t read_msr(void);
static void reset_controller(void);
static void wait_for_rqm(void);
static void execution_phase(byte*);

#endif