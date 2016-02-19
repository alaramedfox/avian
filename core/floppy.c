#define FLOPPY_C_SOURCE
#include <floppy.h>
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/floppy.c
 *		Purpose:	Floppy driver
 */
#include <util.h>
#include <mmap.h>
#include <pic.h>

enum __FLOPPY_DATA
{
	CYL = 80,	//Cylinders
	HPC = 2,	//Headers per cylinder
	SPT = 18,	//Sectors per track
};

/*		
 *	Floppy Ports:
 *		Read-Only status registers	
 *		- SRA, SRB, MSR (Main Status Register), DIR
 *		- MSR contains all the "busy" flags. Check it OFTEN!!
 *		
 *		Write-only config registers
 *		- DSR, CCR (Both should be set to 0!!)
 *
 *		Data registers
 *		- DOR (Digital Output Register): Drive motors
 *		- FIFO: All I/O data transfer
 */
enum __DOR_BITFLAGS
{
	/* Drive Motor on/off */
	MOTD = 0x80, MOTC = 0x40, MOTB = 0x20, MOTA = 0x10,
	
	IRQBIT = 0x08, //IRQ/DMA enabled bit
	RESET  = 0x04, //0 to enter RESET mode; 1 otherwise
	DSEL 	 = 0x03, //Drive selection
};

enum __MSR_BITFLAGS
{
	RQM = 0x80, //FIFO data I/O READY
	DIO = 0x40, //FIFO command READY
	CB  = 0x10, //Busy: executing command
	
	/* Drive Seek flags */
	ACTD = 0x08, ACTC = 0x04, ACTB = 0x02, ACTA = 0x01,
};

static bool floppy_irq_recieved = false;

/* Called when IRQ6 is generated */
void floppy_handler(void)
{
	floppy_irq_recieved = true;
}


status_t floppy_init(void)
{
	/* Enable FDC irq signal */
	pic_enable_irq( IRQ_FLOPPY );
	
	/* Init CCR & DSR for 1.44M Floppies */
	outportb(FDC_CCR,0x00);
	outportb(FDC_DSR,0x00);
	
	return OK;
}

static void floppy_irq_acknowledge(void)
{
	floppy_irq_recieved = false;
	pic_send_eoi( IRQ_FLOPPY );
}	

static void __floppy_reset(void)
{

}

static byte __floppy_read_byte(void);

static chs_t __lba_convert(dword lba)
{
	chs_t chs;
	chs.cyl	= lba / (2 * SPT);
   chs.head = ((lba % (2 * SPT)) / SPT);
   chs.sect = ((lba % (2 * SPT)) % SPT + 1);
}








