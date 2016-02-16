#define FLOPPY_C_SOURCE
#include <floppy.h>
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/floppy.c
 *		Purpose:	Floppy driver
 */

enum __FLP_PORTS
{
	STATUS_A      = 0x3F0, // read-only
	STATUS_B      = 0x3F1, // read-only
	DOUT          = 0x3F2,
	TAPE_DRIVE    = 0x3F3,
	STATUS_MAIN   = 0x3F4, // read-only
	RATE          = 0x3F4, // write-only
	DATA_FIFO     = 0x3F5,
	DIN           = 0x3F7, // read-only
	CONFIG        = 0x3F7  // write-only
};

/* Public API functions */

byte floppy_write(block_t data, addr_t location)
{
	
}

block_t floppy_read(addr_t location)
{

}