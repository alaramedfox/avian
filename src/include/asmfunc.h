#ifndef ASM_FUNC_H_INCLUDED
#define ASM_FUNC_H_INCLUDED
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/asm/asmfunc.h
//      Purpose:         Header for all global assembly functions
// ======================================================================== */

#include <stdlib.h>

/* Define a list of usable ports for bus IO */
enum __PORTS
{
   /* Programmable Interrupt Controller */
   PIC1_CMD = 0x20, PIC1_DAT = 0x21, PIC2_CMD = 0xA0, PIC2_DAT = 0xA1,
   
   /* PS/2 Controller - Keyboard & Mice */
   KB_DATA = 0x60, KB_STATUS = 0x64,
   
   /* Floppy Drive Controller */
   FDC_SRA = 0x3f0, FDC_SRB = 0x3f1, FDC_MSR  = 0x3f4, FDC_DIR = 0x3f7,
   FDC_DSR = 0x3f4, FDC_CCR = 0x3f7, FDC_FIFO = 0x3f5, FDC_DOR = 0x3f2,

};

/* Port IO, defined in portio.asm */
extern byte inportb(word);
extern void outportb(word, byte);

/* Interrupt controllers, defined in interrupts.asm */
extern void load_idt(dword*);
extern void keyboard_irq(void);
extern void floppy_irq(void);
extern void pit_irq(void);

/* Exception controllers, defined in throw.asm */
extern void throw_zero_divide(void);


#endif