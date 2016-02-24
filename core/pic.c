#define PIC_C_SOURCE
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/pic.c
 *		Purpose:	Programmable Interrupt Controller
 */
 
#include <pic.h>
#include <asmfunc.h>
#include <util.h>
#include <stdio.h>

#define __fetch_irq_mask(_PORT, _IRQ, _MASK)	\
	port_t _PORT;	 							  		\
	if(_IRQ < 8) { 									\
		_PORT = PIC1_DAT; 							\
	} 														\
	else { 												\
		_PORT = PIC2_DAT;								\
		_IRQ -= 8; 										\
	} 														\
	byte _MASK = inportb(_PORT)

void pic_init(void)
{
	print("Starting the Programmable Interrupt Controller\n");
	pic_send_command(PIC_INIT);		// ICW1 - Begin init
	pic_send_data(OFFSET1, OFFSET2);	// ICW2 - Remap IDT
	pic_send_data(0x00, 0x00);			// ICW3 - Setup cascading
	pic_send_data(0x01, 0x01);			// ICW4 - Enviornment info
	
	pic_send_data(0xFF, 0xFF);
}

void pic_send_eoi(interrupt_t irq)
{
	/* If using PIC2, EOI must be sent to both PICs */
	if(irq >= 8) { outportb( PIC2_CMD, PIC_EOI ); }
	outportb( PIC1_CMD, PIC_EOI );
}

void pic_enable_irq(interrupt_t irq)
{
	uint16_t port;
    uint8_t value;
 
    if(irq < 8) {
        port = PIC1_DAT;
    } else {
        port = PIC2_DAT;
        irq -= 8;
    }
    value = inportb(port) & ~(1 << irq);
    outportb(port, value);   
}

/*
__fetch_irq_mask(port, irq, mask);
	mask bitmask(irq);
	print(itoa(mask,HEX));
	outportb(port, mask);
	*/

void pic_disable_irq(interrupt_t irq)
{
	uint16_t port;
    uint8_t value;
 
    if(irq < 8) {
        port = PIC1_DAT;
    } else {
        port = PIC2_DAT;
        irq -= 8;
    }
    value = inportb(port) | (1 << irq);
    outportb(port, value);
}

/*
void pic_disable_irq(interrupt_t irq)
{
	__fetch_irq_mask(port, irq, mask);
	mask bitset(irq);
	outportb(port, mask);
}
*/

static inline void pic_send_command(pic_cmd_t cmd)
{	
	outportb(PIC1_CMD, cmd);
	outportb(PIC2_CMD, cmd);
}

static inline void pic_send_data(byte data1, byte data2)
{
	outportb(PIC1_DAT, data1);
	outportb(PIC2_DAT, data2);
}

void pic_remap(void)
{
	/* Save current IRQ masks */
	byte mask1 = inportb(PIC1_DAT);
	byte mask2 = inportb(PIC2_DAT);
	
	pic_init();
	
	pic_send_data(mask1, mask2); 		// Restore saved IRQ masks
}





