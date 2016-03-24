#ifndef FLOPPY_H_INCLUDED
#define FLOPPY_H_INCLUDED
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/drivers/floppy.h
//      Purpose:         Header for the floppy controller
// ======================================================================== */

#include <stdlib.h>
#define floppy_   FDC_
 
/* Floppy CHS Address datatype */
typedef struct __CHS
{
   word cyl, head, sect;

} chs_t;

enum __FLOPPY_ERR
{
   FDC_OK    = 0, // Sucessful execution
   FDC_IOERR = 1, // I/O Failure
   FDC_UNSUP = 2, // Floppy device not supported
   FDC_TMO   = 3, // IRQ timed out
   FDC_STATUS = 4, // Bad status value
};

#define GAP3 0x1B

/* Function called by Interrupt Controller */
void floppy_handler(void);
 
/* Public API */
void floppy_init(void);
int floppy_read_block(word, byte*, size_t);
int floppy_write_block(word, byte*, size_t);




#endif