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
#include <vga.h>
#include <time.h>
#include <idt.h>
#include <asmfunc.h>
#include <dma.h>

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
	MOTD=7, MOTC=6, MOTB=5, MOTA=4,
	IRQBIT=3,		//IRQ/DMA enabled bit
	RESET=2, 		//0 to enter RESET mode; 1 otherwise
	DSEL1=1, 		//Drive selection
	DSEL0=0,
};

enum __MSR
{
	RQM=7,	//FIFO data I/O READY
	DIO=6, 	//FIFO command READY
	CB=5,	//Busy: executing command
	NDMA=4,	//PIO mode execution flag
	/* Drive seek flags */
	ACTD=3, ACTC=2, ACTB=1, ACTA=0,
};

/* Private variables */
static bool floppy_irq_recieved = false;
static byte floppy_dor_status;
static byte floppy_status;
static byte floppy_current_track;
static byte floppy_io_buffer[7];

bool floppy_write_block(int lba, byte *block, size_t sectors)
{
	//ASSERT("Floppy write", lba, sectors, HEX);
	return floppy_data_transfer(lba, block, false, sectors);
}

bool floppy_read_block(int lba, byte *block, size_t sectors)
{
	//ASSERT("Floppy read", lba, sectors, HEX);
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
	//ASSERT("Caught floppy IRQ",0,0,DEC);
	floppy_irq_recieved = true;
	pic_send_eoi( IRQ_FLOPPY );
}


void floppy_init(void)
{
	/* Enable FDC irq signal */
	idt_add_handler((addr_t)floppy_irq, IRQ_FLOPPY);
	pic_enable_irq( IRQ_FLOPPY );
	
	floppy_configure();
	floppy_reset();
	
	/* Init CCR & DSR for 1.44M Floppies */
	outportb(FDC_CCR,0x00);
	outportb(FDC_DSR,0x00);
	
	print_time(); print("Started floppy controller\n");
}

static void floppy_reset(void)
{
	floppy_irq_recieved = false;
	
	/* Disable interrupts and shut off all motors */
	floppy_dor_status = 0;
	outportb(FDC_DOR, floppy_dor_status);
	
	/* Reset datarate */
	outportb(FDC_DSR, 0);
	
	/* Re-enable interrupts */
	floppy_dor_status = bitset(floppy_dor_status, IRQBIT);
	floppy_dor_status = bitset(floppy_dor_status, 4);
	outportb(FDC_DOR, floppy_dor_status);
	
	/* Handle reset interrupt */
	floppy_irq_recieved = true;
	floppy_command_wait(true);
	
	floppy_specify(8,5,0);
	
	floppy_seek(1);
	floppy_recalibrate();
	
}

static void floppy_configure(void)
{
	byte iseek = 1, nofifo = 0, poll = 0, thresh = 8, comp = 0;
	floppy_send_byte(CONFIGURE);
	floppy_send_byte(0);
	floppy_send_byte(iseek<<6 | nofifo<<5 | poll<<4 | (thresh-1));
	floppy_send_byte(comp);
}

static void floppy_specify(byte srt, byte hlt, byte hut)
{
	/* 8 5 0 */
	floppy_send_byte(SPECIFY);
	floppy_send_byte(srt << 4 | hut);
	floppy_send_byte(hlt << 1 | 0 );
}

static void floppy_recalibrate(void)
{
	floppy_start_motor(0);
	
	floppy_send_byte(RECALIBRATE);
	floppy_send_byte(0);
	
	if(!floppy_command_wait(true)) {
		print("Recalibration failed\n");
	}
	
	floppy_stop_motor(0);
	//floppy_specify(8,5,0);
}

static int floppy_seek(byte track)
{
	//ASSERT("floppy_seek() (0/track)", 0, track, DEC);
	if(floppy_current_track == track) return true;
	
	floppy_start_motor(0);
	
	floppy_send_byte(SEEK);
	floppy_send_byte(0);
	floppy_send_byte(track);
	
	int time = clock();
	
	while(!floppy_command_wait(true)) {
		if(clock() > time+3000) return -1; // IRQ timed out
	}
	//print("Caught IRQ after "); print(itoa(clock()-time,DEC)); print(" ms\n");
	
	sleep(20); // Let head settle
	
	floppy_stop_motor(0);
	
	/* Ensure that the seek worked */
	if((floppy_status != 0x20) || (floppy_current_track != track)) {
		return -2; // Track align fail
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
	//ASSERT("Waiting for command to finish (IRQ/sensei)",floppy_irq_recieved,sensei,BOOLEAN);
	
	/* Wait for interrupt to arrive */
	int time = clock();
	
	while(!floppy_irq_recieved)
	{
		if(clock() > time+500) return false;
	}
	
	/* Read in the command result bytes */
	for(int i=0; i<7 && (bitcheck(inportb(FDC_MSR), CB)); i++) {
		
		floppy_io_buffer[i] = floppy_read_byte();
		//ASSERT("Read FIFO byte:",i,floppy_io_buffer[i],HEX);
	}
	
	/* Detect interrupt and gather status data */
	if(sensei) {
		for(int i=0; i<4; i++)
		{
			floppy_send_byte(SENSEI);
			floppy_status = floppy_read_byte();
			floppy_current_track = floppy_read_byte();
		}
		//ASSERT("SENSEI Status: (st0, track)", floppy_status, floppy_current_track, HEX);
	}
	floppy_irq_recieved = false;
	return true;
}

static byte floppy_read_byte(void)
{
	byte msr;
	
	int time = clock();
	while(clock() < time+500) {
		msr = inportb(FDC_MSR);
	
		if((msr & 0x80) == 0x80) {
			//ASSERT("Reading FIFO byte",msr,0,BIN);
			return inportb(FDC_FIFO);
		}
	}
}

static int floppy_send_byte(byte data)
{
	byte msr;
	
	int time = clock();
	while(clock() < time+500) {
		msr = inportb(FDC_MSR);
	
		if((msr & 0x80) == 0x80) {
			//ASSERT("Writing FIFO byte",msr,0,BIN);
			outportb(FDC_FIFO, data);
			return true;
		}
	}	
	//ASSERT("Failed to send byte to FIFO",inportb(FDC_MSR),0,BIN);
	return false;
}

static bool floppy_data_transfer(int lba, byte *block, bool read, size_t sectors)
{
	//ASSERT("Floppy data transfer",lba,sectors,DEC);
	chs_t chs = lba_convert(lba);
	byte *dma_buffer = (byte*)DMA_BUFFER_ADDRESS;
	
	floppy_start_motor(0);
	
	/* Write data buffer into track buffer */
	if(!read) {
		for(int i=0; i<sectors*512; i++) {
			dma_buffer[i] = block[i];
		}
	}
	
	/* Move to correct track */
	int seek_status = floppy_seek(chs.cyl);
	if(seek_status == -1) {
		print("Floppy error: Seek never caught IRQ\n");
		return false;
	}
	else if(seek_status == -2) {
		print("Floppy error: Seek could not align tracks\n");
		return false;
	}
	
	if(read) {
		/* DMA transfer */
		dma_xfer(2,DMA_BUFFER_ADDRESS,sectors*512,false);
		floppy_send_byte(READ_DATA);
	}
	else {
		/* DMA transfer */
		dma_xfer(2,DMA_BUFFER_ADDRESS,sectors*512,true);
		floppy_send_byte(WRITE_DATA);
	}
	
	/* Send address information */
	floppy_send_byte(chs.head << 2);
	floppy_send_byte(chs.cyl);
	floppy_send_byte(chs.head);
	floppy_send_byte(sectors);
	floppy_send_byte(2);
	floppy_send_byte(SPT);
	floppy_send_byte(GAP3);
	floppy_send_byte(0xFF);
	
	if(!floppy_command_wait(true)) {
		print("Floppy error: NO_IRQ\n");
		return false;
	}
	
	floppy_stop_motor(0);
	
	/* Transfer data from track buffer to data buffer */
	if(read) {
		for(int i=0; i<sectors*512; i++) {
			if(dma_buffer[i] != 0) {
				print("DMA["); print(itoa(i,DEC)); print("] = ");
				print(itoa(dma_buffer[i],HEX));
				print("\n");
			}
			block[i] = dma_buffer[i];
		}
	}
	return true;
}

static void floppy_start_motor(int drive)
{
	floppy_dor_status = bitset(floppy_dor_status, ACTA-drive);
	outportb(FDC_DOR, floppy_dor_status);
	sleep(200);
}

static void floppy_stop_motor(int drive)
{
	floppy_dor_status = bitset(floppy_dor_status, ACTA-drive);
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








