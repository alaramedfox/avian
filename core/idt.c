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

static void idt_add_interrupt(addr_t handler, byte offset)
{
	idt_table[offset].lower 		= handler & 0xFFFF;
	idt_table[offset].selector 	= KERNEL_OFFSET;
	idt_table[offset].zero 			= 0;
	idt_table[offset].type_attr 	= INTERRUPT_GATE;
	idt_table[offset].higher 		= (handler & 0xFFFF0000) >> 16;
	
	idt_write_table();
}

void idt_add_handler(addr_t handler, byte irq)
{
	byte offset = 0x20 + irq;
	idt_add_interrupt(handler, offset);
}

void idt_add_exception(addr_t handler, byte err)
{
	byte offset = err;
	idt_add_interrupt(handler, offset);
}

static inline void idt_write_table(void)
{
	addr_t table_address = (addr_t)idt_table;
	int descriptor[2] = { 
		(sizeof(idt_entry_t) * IDT_SIZE) + ((table_address & 0xffff) << 16),
		table_address >> 16,
	};

	load_idt(descriptor);
}


