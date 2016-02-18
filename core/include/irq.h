#ifndef IRQ_H_INCLUDED
#define IRQ_H_INCLUDED
/*
 *		Avian Kernel - Bryan Webb
 *	  	File:		/core/include/irq.h
 *	  	Purpose: Interrupt Request Handler
 */

#include <types.h>
#include <util.h>

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

void irq_listen(interrupt_t);
void irq_ignore(interrupt_t);


#endif