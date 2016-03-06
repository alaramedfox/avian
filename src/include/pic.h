#ifndef PIC_H_INCLUDED
#define PIC_H_INCLUDED
// ======================================================================== */
//		Avian Kernel   Bryan Webb (C) 2016
//		File:		      /core/include/pic.h
//		Purpose:	      Programmable Interrupt Controller
// ======================================================================== */
 
#include <stdlib.h>

enum __PIC_DEFS
{
	OFFSET1 = 0x20,
	OFFSET2 = 0x28,
};

typedef enum __IRQ_INTERRUPTS
{ 
	/* Cable Port Interrupts */
	IRQ_LPT1 = 7, IRQ_LPT2 = 5,
	IRQ_COM2 = 3, IRQ_COM1 = 4,
	
	/* Clocked Interrupts */
	IRQ_PIT = 0, IRQ_RTC = 8,
	
	/* Peripheral Interrupts */
	IRQ_MOUSE = 12, IRQ_KEYBOARD = 1,
	IRQ9 = 9, IRQ10 = 10, IRQ11 = 11,
	
	/* Drive Storage Interrupts */
	IRQ_FLOPPY = 6,
	IRQ_ATA1 = 14, IRQ_ATA2 = 15,
	
	/* Misc. Interrupts */
	IRQ_CASCADE = 2, IRQ_COPROCESSOR = 13,

} interrupt_t;

typedef enum __PIC_COMMANDS
{
	PIC_INIT = 0x11,
	PIC_EOI = 0x20,
	
} pic_cmd_t;
 
void pic_init(void);
void pic_remap(void);
void pic_send_eoi(interrupt_t);
void pic_enable_irq(interrupt_t);
void pic_disable_irq(interrupt_t);







#endif