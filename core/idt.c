#define IDT_C_SOURCE
/*
 *		Avian Kernel - Bryan Webb
 *	  	File:		/core/idt.c
 *	  	Purpose: Interrupt Descriptor Table
 */
#include <idt.h>

typedef struct __IDT_ENTRY
{
	uint16_t lower;
	uint16_t selector;
	uint8_t  zero;
	uint8_t  type_attr;
	uint16_t higher;
	
} idt_entry_t;

static idt_entry_t idt_table[IDT_SIZE];

void idt_add_handler(addr_t handler, index_t vector, type_attr_t gate)
{
	idt_table[vector].lower 		= handler & 0xFFFF;
	idt_table[vector].selector 	= KERNEL_OFFSET;
	idt_table[vector].zero 			= 0;
	idt_table[vector].type_attr 	= gate;
	idt_table[vector].higher 		= (handler & 0xFFFF0000) >> 16;
}

void idt_init(void)
{
	/* ICW1 - begin initialization */
	outportb( PIC1_CMD , 0x11);
	outportb( PIC2_CMD , 0x11);

	/* ICW2 - remap offset address of IDT
	 * In x86 protected mode, we have to remap the PICs beyond 0x20 because
	 * Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	 */
	outportb( PIC1_DAT , 0x20);
	outportb( PIC2_DAT , 0x28);

	/* ICW3 - setup cascading */
	outportb( PIC1_DAT , 0x00);
	outportb( PIC2_DAT , 0x00);

	/* ICW4 - environment info */
	outportb( PIC1_DAT , 0x01);
	outportb( PIC2_DAT , 0x01);
	/* Initialization finished */

	/* mask interrupts */
	outportb( PIC1_DAT , 0xff);
	outportb( PIC2_DAT , 0xff);

	/* fill the IDT descriptor */
	addr_t iaddr = (addr_t)idt_table;
	int iptr[2] = { 
		(sizeof(idt_entry_t) * IDT_SIZE) + ((iaddr & 0xffff) << 16),
		iaddr >> 16,
	};

	load_idt(iptr);
}
