#define IRQ_C_SOURCE
/*
 *		Avian Kernel - Bryan Webb
 *	  	File:		/core/irq.c
 *	  	Purpose: Interrupt Request Handler
 */
 
#include <irq.h>
#include <asmfunc.h>
#include <util.h>
 
void irq_listen(interrupt_t line)
{
	port_t port;
	byte imr;
	
	if(line < 8) {
		port = PIC1_DAT;
	}
	else {
		port = PIC2_DAT;
		line = line-8;
	}
	
	imr = inportb(port);
	imr bitmask(line);
}

void irq_ignore(interrupt_t line)
{

}