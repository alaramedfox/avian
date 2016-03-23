#define FLOPPY_C_SOURCE

// ========================================================================= //
//      Avian Kernel   Bryan Webb (C) 2016
//      File:          /core/floppy.c
//      Purpose:       Floppy Disk Controller (FDC)
// ========================================================================= //

#include <floppy.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <errors.h>
#include <asmfunc.h>
#include <pic.h>
#include <time.h>
#include <idt.h>
#include <dma.h>
#include <vga.h>
#include <util.h>

enum __FLOPPY_DATA
{
   CYL = 80,   //Cylinders
   HPC = 2,      //Headers per cylinder
   SPT = 18,   //Sectors per track
};

enum __FLOPPY_COMMANDS
{
   READ_TRACK =   2,      // generates IRQ6
   SPECIFY =      3,    // * set drive parameters
   WRITE_DATA =   5,    // * write to the disk
   READ_DATA =    6,    // * read from the disk
   RECALIBRATE =  7,    // * seek to cylinder 0
   SENSEI =       8,    // * ack IRQ6, get status of last command
   READ_ID =      10,   // generates IRQ6
   FORMAT_TRACK = 13,   // *
   SEEK =         15,   // * seek both heads to cylinder X
   VERSION =      16,   // * used during initialization, once
   MODE_P =       18,   // * used during initialization, once, maybe
   CONFIGURE =    19,   // * set controller parameters
   LOCK =         20,   // * protect controller params from a reset
};

//   ========================================================================= */
//      Floppy Ports:
//      Read-Only status registers   
//         - SRA, SRB, MSR (Main Status Register), DIR
//         - MSR contains all the "busy" flags. Check it OFTEN!!
//      
//      Write-only config registers
//         - DSR, CCR (Both should be set to 0!!)
//
//      Data registers
//         - DOR (Digital Output Register): Drive motors
//         - FIFO: All I/O data transfer
// ========================================================================= */

enum __DOR
{
   /* Drive Motor on/off */
   MOTD=7, MOTC=6, MOTB=5, MOTA=4,
   IRQBIT=3,      //IRQ/DMA enabled bit
   RESET=2,       //0 to enter RESET mode; 1 otherwise
   DSEL1=1,       //Drive selection
   DSEL0=0,
};

enum __MSR
{
   RQM=7,   //FIFO data I/O READY
   DIO=6,    //FIFO command READY
   CB=5,      //Busy: executing command
   NDMA=4,   //PIO mode execution flag
   /* Drive seek flags */
   ACTD=3, ACTC=2, ACTB=1, ACTA=0,
};

// ========================================================================= //
//           Private Variables and Subroutines                               //
// ========================================================================= //

/* Private variables */
static volatile bool floppy_irq_recieved = false;
static bool floppy_supported;
static byte floppy_dor_status;
static byte floppy_status;
static byte floppy_current_track;
static byte floppy_io_buffer[7];
static byte floppy_motor_count=0;

/* Private functions */
static void  floppy_sense_interrupt(void);
static void  floppy_configure(void);
static void  floppy_specify(byte,byte,byte);
static int   floppy_data_transfer(int,byte*,size_t,bool);
static int   floppy_seek(byte);
static void  floppy_recalibrate(void);
static void  floppy_reset(void);
static bool  floppy_command_wait(int);
static void  floppy_start_motor(int);
static void  floppy_stop_motor(int);
static int   floppy_send_byte(byte);
static int   floppy_read_byte(byte*);
static chs_t lba_convert(int);

// ========================================================================= //
//           Public API Definitions                                          //
// ========================================================================= //

int floppy_write_block(word lba, byte *block, size_t bytes)
{
   return floppy_data_transfer(lba, block, bytes, false);
}

int floppy_read_block(word lba, byte *block, size_t bytes)
{
   return floppy_data_transfer(lba, block, bytes, true);
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
   idt_add_handler((addr_t)floppy_irq, IRQ_FLOPPY);
   pic_enable_irq( IRQ_FLOPPY );
   
   /* Obtain controller version */
   byte version=0;
   floppy_send_byte(VERSION);
   floppy_read_byte(&version);
   
   if(version == 0x90) {
      throw("Detected 82077AA floppy controller", 0);
      floppy_supported = true;
   }
   else {
      throw("Unsupported floppy controller", 1);
      floppy_supported = false;
      return;
   }
   floppy_configure();
   floppy_recalibrate();
   floppy_send_byte(LOCK);
   floppy_reset();
   
   /* Init CCR & DSR for 1.44M Floppies */
   outportb(FDC_CCR,0x00);
   outportb(FDC_DSR,0x00);
   
   throw("Started floppy controller",0);
}

static void floppy_reset(void)
{  
   floppy_irq_recieved = false;
   
   /* Disable interrupts and shut off all motors */
   floppy_motor_count = 0;
   outportb(FDC_DOR, 0);
   sleep(4);
   
   /* Reset datarate */
   outportb(FDC_DSR, 0);
   
   /* Re-enable interrupts and select drive 0 */
   floppy_dor_status = 0x0D;
   outportb(FDC_DOR, floppy_dor_status);
   
   /* Handle reset interrupt */
   floppy_command_wait(500);
   floppy_specify(8,5,0);
}

static void floppy_configure(void)
{  
   byte iseek = 1, nofifo = 0, poll = 0, thresh = 8, comp = 0;
   outportb(FDC_FIFO, CONFIGURE);
   outportb(FDC_FIFO, 0);
   outportb(FDC_FIFO, iseek<<6 | nofifo<<5 | poll<<4 | (thresh-1));
   outportb(FDC_FIFO, comp);
}

static void floppy_specify(byte srt, byte hlt, byte hut)
{  
   outportb(FDC_FIFO, SPECIFY);
   outportb(FDC_FIFO, srt << 4 | hut);
   outportb(FDC_FIFO, hlt << 1 | 0 );
}

static void floppy_recalibrate(void)
{  
   floppy_start_motor(0);
   floppy_send_byte(RECALIBRATE);
   floppy_send_byte(0);
   
   if(!floppy_command_wait(3000)) {
      throw("Recalibration failed",1);
   }
   
   floppy_sense_interrupt();
   floppy_stop_motor(0);
}

static int floppy_seek(byte track)
{     
   if(!floppy_supported) return FDC_UNSUP;
   if(floppy_current_track == track) return FDC_OK;
   
   floppy_start_motor(0);
   
   floppy_send_byte(SEEK);
   floppy_send_byte(0);
   floppy_send_byte(track);
   
   volatile byte msr;
   do {
      msr = inportb(FDC_MSR);
      
   } while(msr & 0x0F);
   
   if(!floppy_command_wait(3000)) {
      floppy_sense_interrupt();
      floppy_reset();
      floppy_stop_motor(0);
      //throw(this, FDC_TMO);
      return FDC_TMO; // IRQ timed out
   }
   
   floppy_sense_interrupt();
   
   floppy_stop_motor(0);
   
   /* Ensure that the seek worked */
   if(floppy_current_track != track) {
      throw("FDC Track Align", 1);
      return FDC_IOERR; // Track align fail
   }
   else if(floppy_status != 0x20) {
      throw("FDC Bad Status", 1);
      return FDC_STATUS; // Bad status
   }
   else {
      return FDC_OK;
   }
}

static void floppy_sense_interrupt(void)
{  
   outportb(FDC_FIFO, SENSEI);
   floppy_status = inportb(FDC_FIFO);
   floppy_current_track = inportb(FDC_FIFO);
}

/**
 *      Used after sending a command.
 *      Waits for the IRQ signal, then reads in
 *      the result bytes into the buffer.
 */
static bool floppy_command_wait(int ms)
{   
   bool status = true;
   timeout_ms(status, ms, 
      if(floppy_irq_recieved) {
         status = true;
         break;
      } 
   );
   
   /* Read in the command result bytes */
   for(int i=0; i<7 && (bitcheck(inportb(FDC_MSR), CB)); i++) {
      floppy_read_byte(&floppy_io_buffer[i]);
   }
   floppy_irq_recieved = false;
   return status;
}

static int floppy_read_byte(byte* data)
{  
   
   volatile byte msr;
   bool status;
   timeout_ms(status, 500,
      msr = inportb(FDC_MSR);
   
      if((msr & 0xC0) == 0xc0) {
         *data = inportb(FDC_FIFO);
         return FDC_OK;
      }
      floppy_reset();
   );
   if(!status) throw("FDC I/O Error",2);
   return FDC_IOERR;
}

static int floppy_send_byte(byte data)
{  
   volatile byte msr;
   
   bool status;
   timeout_ms(status, 500,
      msr = inportb(FDC_MSR);
   
      if((msr & 0xC0) == 0x80) {
         outportb(FDC_FIFO, data);
         return FDC_OK;
      }
      floppy_reset();
   );
   if(!status) throw("FDC I/O Error",2);  
   return FDC_IOERR;
}

#include <vga.h>
static int floppy_data_transfer(int lba, byte *block, size_t bytes, bool read)
{
   bool status = FDC_OK;
   chs_t chs = lba_convert(lba);
   byte *dma_buffer = (byte*) malloc(bytes);
   
   printf("Floppy: Buffer %#%s%# sector %i\n", BROWN,read?"<--":"-->",C_TERM,lba);
   
   floppy_start_motor(0);
   
   /* Write data buffer into track buffer */
   if(!read) {
      memcpy(dma_buffer, block, bytes);
   }
   
   /* Move to correct track */
   //print("Seeking\n");
   int tries = 3;
   int seek_status;
   do {
      seek_status = floppy_seek(chs.cyl);
      tries--;
      
   } while(tries && seek_status);
   
   if(seek_status) {
      status = seek_status;
      throw("FDC Seek Failed", 3);
      goto exit;
   }
   
   /**
    * Prepare the DMA for a data transfer on channel 2
    * Note: For some reason, the read/write flag is INVERTED
    * in the DMA transfer function.
    */
   dma_xfer(2, (addr_t)dma_buffer, bytes, !read);
   
   if(read) {
      status = floppy_send_byte(READ_DATA | 0x40);
   }
   else {
      status = floppy_send_byte(WRITE_DATA | 0x40);
   }
   
   if(status) {
      goto exit;
   }
   
   /* Send address information */
   floppy_send_byte(chs.head << 2);
   floppy_send_byte(chs.cyl);
   floppy_send_byte(chs.head);
   floppy_send_byte(chs.sect);
   floppy_send_byte(2);
   floppy_send_byte(bytes/512);
   floppy_send_byte(GAP3);
   floppy_send_byte(0xFF);
   
   //print("Waiting for IRQ\n");
   
   floppy_command_wait(1000);
   
   /* Transfer data from track buffer to data buffer */
   if(read) {
      memcpy(block, dma_buffer, bytes);
   }
   
   exit:
   floppy_stop_motor(0);
   free(dma_buffer);
   return status;
}

static void floppy_start_motor(int drive)
{  
   if(floppy_motor_count == 0) {
      floppy_dor_status = 0xFD;
      outportb(FDC_DOR, floppy_dor_status | drive);
      sleep(200);
   }
   floppy_motor_count++;
}

static void floppy_stop_motor(int drive)
{  

   if(floppy_motor_count == 0) {
      floppy_dor_status = 0x0D;
      outportb(FDC_DOR, floppy_dor_status | drive);
   }
   else floppy_motor_count--;
}

static chs_t lba_convert(int lba)
{
   chs_t chs;
   chs.cyl   = lba / (2 * SPT);
   chs.head = ((lba % (2 * SPT)) / SPT);
   chs.sect = ((lba % (2 * SPT)) % SPT + 1);
   return chs;
}
