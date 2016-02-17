#define FLOPPY_C_SOURCE
#include <floppy.h>
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/floppy.c
 *		Purpose:	Floppy driver
 */
 
#include <envar.h>
#include <util.h>

enum __FLOPPY_DATA
{
	CYL=80,	//Cylinders
	HPC=2,	//Headers per cylinder
	SPT=18,	//Sectors per track
};

/* Public API functions */

void floppy_init(void)
{
	/* Set DSR register to 0 for use with 1.44 floppies */
	ASM_outb( FDC_DSR, 0x00 );
	ASM_outb( FDC_CCR, 0x00 );
}

byte floppy_write(block_t data, addr_t location)
{
	
}

block_t floppy_read(addr_t location)
{

}

/******* Static helper functions **********/

static msr_bitflag_t read_msr(void)
{
	byte msr_data = ASM_inb( FDC_MSR );
	msr_bitflag_t *msr_bits = (msr_bitflag_t*)&msr_data;
	return *msr_bits;
}

static void issue_command(byte com, byte *param, byte *result)
{
	msr_bitflag_t msr = read_msr();
	
	if( msr.RQM != 1 || msr.DIO != 0) {
		reset_controller();
		issue_command(com,param,result);
	}
	else {
		ASM_outb( FDC_FIFO, com ); //Issue command byte
		issue_command_args(param); //Issue parameters
		msr = read_msr();
		if(msr.NDMA == 1) {
			execution_phase(result);
		}
		else {
			/* Skip to result phase */
		}
		
	}
}

static inline void execution_phase(byte *buffer)
{
	msr_bitflag_t msr = read_msr();
	
	 while(true)
	{
		wait_for_rqm();
		while(msr.RQM == 1)
		{
			
		}
	}
	
}

static inline void wait_for_rqm(void)
{
	until (read_msr().RQM == 1);
	return;
}

static inline bool issue_command_args(byte *param)
{
	msr_bitflag_t msr = read_msr();
	
	for(int i=0; param[i] != 0xFF; i++) {
		wait_for_rqm();
		
		/* Check if FIFO is expecting an opcode */
		if(msr.DIO == 0) {
			ASM_outb( FDC_FIFO, param[i] );
		}
	   else {
	   	ENVAR.GLOBAL.status = WARN("[ FDC ] Invalid opcode");
	   	return false;
	   }
	}
	return true;
}

static void reset_controller(void)
{

}

static chs_t lba_convert(dword lba)
{
	chs_t chs;
	chs.cyl	= lba / (2 * SPT);
   chs.head = ((lba % (2 * SPT)) / SPT);
   chs.sect = ((lba % (2 * SPT)) % SPT + 1);
}

static void motor_power(byte drive, bool power)
{
	byte dor_data = ASM_inb(FDC_DOR);
	dor_bitflag_t *dor_bits = (dor_bitflag_t*)&dor_data;
	
	switch(drive)
	{
		case 0: dor_bits->MOTA=power; break;
		case 1: dor_bits->MOTB=power; break;
		case 2: dor_bits->MOTC=power; break;
		case 3: dor_bits->MOTD=power; break;
		default: break;
	}
	
	ASM_outb(FDC_DOR, dor_data);
}






