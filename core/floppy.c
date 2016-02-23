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

enum __FLOPPY_COMMANDS
{
   READ_TRACK =   2,		// generates IRQ6
   SPECIFY =      3,    // * set drive parameters
   WRITE_DATA =   5,    // * write to the disk
   READ_DATA =    6,    // * read from the disk
   RECALIBRATE =  7,    // * seek to cylinder 0
   SENSEI =       8,    // * ack IRQ6, get status of last command
   READ_ID =      10,	// generates IRQ6
   FORMAT_TRACK = 13,   // *
   SEEK =         15,   // * seek both heads to cylinder X
   VERSION =      16,	// * used during initialization, once
   MODE_P =       18,	// * used during initialization, once, maybe
   CONFIGURE =    19,   // * set controller parameters
   LOCK =         20,   // * protect controller params from a reset
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
enum __DOR
{
	/* Drive Motor on/off */
	MOTD, MOTC, MOTB, MOTA,
	IRQBIT,		//IRQ/DMA enabled bit
	RESET, 		//0 to enter RESET mode; 1 otherwise
	DSEL1, 		//Drive selection
	DSEL0,
};

enum __MSR
{
	RQM,	//FIFO data I/O READY
	DIO, 	//FIFO command READY
	CB,	//Busy: executing command
	NDMA,	//PIO mode execution flag
	/* Drive seek flags */
	ACTD, ACTC, ACTB, ACTA,
};

/* Private variables */
static bool floppy_irq_recieved = false;
static byte floppy_dor_status;
static byte floppy_status;
static byte floppy_current_track;
static byte floppy_io_buffer[7];

bool floppy_write_block(int lba, block_t *block, size_t sectors)
{
	return floppy_data_transfer(lba, block, false, sectors);
}

bool floppy_read_block(int lba, block_t *block, size_t sectors)
{
	chs_t start = lba_convert(lba);
	chs_t end = lba_convert(lba+sectors);
	
	if(start.cyl != end.cyl)
	{
		bool result;
		for(int i=0; i<sectors; i++) {
			result = floppy_data_transfer(lba+i, block+(i*512), true, 1);
		}
		return result;
	}
	
	return floppy_data_transfer(lba, block, true, sectors);
}

/* Called when IRQ6 is generated */
void floppy_handler(void)
{
	floppy_irq_recieved = true;
	pic_send_eoi( IRQ_FLOPPY );
}


void floppy_init(void)
{
	/* Enable FDC irq signal */
	pic_enable_irq( IRQ_FLOPPY );
	
	/* Init CCR & DSR for 1.44M Floppies */
	outportb(FDC_CCR,0x00);
	outportb(FDC_DSR,0x00);
}

static void floppy_reset(void)
{
	/* Disable interrupts and shut off all motors */
	floppy_dor_status = 0;
	outportb(FDC_DOR, floppy_dor_status);
	
	/* Reset datarate */
	outportb(FDC_DSR, 0);
	
	/* Re-enable interrupts */
	floppy_dor_status = floppy_dor_status bitset(IRQBIT);
	outportb(FDC_DOR, floppy_dor_status);
	
	/* Handle reset interrupt */
	floppy_irq_recieved = true;
	floppy_command_wait(true);
	
	/* TODO: Specify drive timing */
	
	/* TODO: Seek(1), recalibrate(); */
	
}

static void floppy_recalibrate(void)
{
	floppy_start_motor(0);
	
	floppy_send_byte(RECALIBRATE);
	floppy_send_byte(0);
	
	floppy_command_wait(true);
	
	floppy_stop_motor(0);
}

static bool floppy_seek(byte track)
{
	if(floppy_current_track == track) return true;
	
	floppy_start_motor(0);
	
	floppy_send_byte(SEEK);
	floppy_send_byte(0);
	floppy_send_byte(track);
	
	if(!floppy_command_wait(true)) return false;
	
	/* TODO: set delay! */
	
	floppy_stop_motor(0);
	
	/* Ensure that the seek worked */
	if((floppy_status != 0x20) || (floppy_current_track != track)) {
		return false;
	}
	else {
		return true;
	}
}

/*
 *	floppy_command_wait():
 *		Used after sending a command.
 *		Waits for the IRQ signal, then reads in
 *		the result bytes into the buffer.
 *
 *		TODO: Add timeouts and error codes
 */
static bool floppy_command_wait(bool sensei)
{
	/* Wait for interrupt to arrive */
	while(!floppy_irq_recieved);
	
	/* Read in the command result bytes */
	for(int i=0; i<7 && (inportb(FDC_MSR) bitcheck(CB)); i++) {
		floppy_io_buffer[i] = floppy_read_byte();
	}
	
	/* Detect interrupt and gather status data */
	if(sensei) {
		floppy_send_byte(SENSEI);
		floppy_status = floppy_read_byte();
		floppy_current_track = floppy_read_byte();
	}
	
	floppy_irq_recieved = false;
}

static byte floppy_read_byte(void)
{
	byte msr = inportb(FDC_MSR);
	bool rqm = msr bitcheck(RQM);
	bool dio = msr bitcheck(DIO);
	bool cb  = msr bitcheck(CB);
	
	if(rqm && !dio && cb) {
		return inportb(FDC_FIFO);
	}
}

static int floppy_send_byte(byte data)
{
	byte msr = inportb(FDC_MSR);
	bool rqm = msr bitcheck(RQM);
	bool dio = msr bitcheck(DIO);
	
	if(rqm && !dio) {
		outportb(FDC_FIFO, data);
		return 1;
	}
	
	return 0;
}

static bool floppy_data_transfer(int lba, block_t *block, bool read, size_t sectors)
{
	chs_t chs = lba_convert(lba);
	byte *dma_buffer = (byte*)DMA_BUFFER_ADDRESS;
	
	floppy_start_motor(0);
	
	/* Write data buffer into track buffer */
	if(!read) {
		for(int i=0; i<sectors*512; i++) {
			dma_buffer[i] = block->data[i];
		}
	}
	
	/* Move to correct track */
	floppy_seek(chs.cyl);
	
	if(read) {
		/* DMA transfer */
		floppy_send_byte(READ_DATA);
	}
	else {
		/* DMA transfer */
		floppy_send_byte(WRITE_DATA);
	}
	
	/* Send address information */
	floppy_send_byte(chs.head << 2);
	floppy_send_byte(chs.cyl);
	floppy_send_byte(chs.head);
	floppy_send_byte(sectors);
	floppy_send_byte(2);
	floppy_send_byte(SPT);
	/* TODO: send gap size data */
	floppy_send_byte(0xFF);
	
	floppy_command_wait(true);
	
	floppy_stop_motor(0);
	
	/* Transfer data from track buffer to data buffer */
	if(read) {
		for(int i=0; i<sectors*512; i++) {
			block->data[i] = dma_buffer[i];
		}
	}
}

static void floppy_start_motor(int drive)
{
	floppy_dor_status = floppy_dor_status bitset(ACTA-drive);
	outportb(FDC_DOR, floppy_dor_status);
}

static void floppy_stop_motor(int drive)
{
	floppy_dor_status = floppy_dor_status bitmask(ACTA-drive);
	outportb(FDC_DOR, floppy_dor_status);
}

static chs_t lba_convert(int lba)
{
	chs_t chs;
	chs.cyl	= lba / (2 * SPT);
   chs.head = ((lba % (2 * SPT)) / SPT);
   chs.sect = ((lba % (2 * SPT)) % SPT + 1);
   return chs;
}








