#define IDT_C_SOURCE
#include <idt.h>

/*
 *	Popcorn Kernel
 *	  File:		/core/idt.c
 *	  Purpose: 	Interrupt handler
 */
 
struct __IDT_ENTRY
{
	uint16_t lower;
	uint16_t selector;
	uint8_t  zero;
	uint8_t  type_attr;
	uint16_t higher;
};

struct __IDT_ENTRY idt_table[IDT_SIZE];

void idt_init(void)
{
	int32_t kaddr;
	int32_t iaddr;
	int32_t iptr[2];

	/* Populate IDT entry of keyboard's interrupt */
	kaddr = (int32_t)keyboard_handler;
	idt_table[0x21].lower 		= kaddr & 0xFFFF;
	idt_table[0x21].selector 	= KERNEL_CODE_SEGMENT_OFFSET;
	idt_table[0x21].zero			= 0;
	idt_table[0x21].type_attr 	= INTERRUPT_GATE;
	idt_table[0x21].higher 		= (kaddr & 0xFFFF0000) >> 16;

	/*
	 *	Port    	PIC1	PIC2
	 *	Command 	0x20	0xA0
	 *	Data	 	0x21	0xA1
	 */

	/* ICW1 - begin initialization */
	ASM_write_port(0x20 , 0x11);
	ASM_write_port(0xA0 , 0x11);

	/* ICW2 - remap offset address of IDT
	 * In x86 protected mode, we have to remap the PICs beyond 0x20 because
	 * Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	 */
	ASM_write_port(0x21 , 0x20);
	ASM_write_port(0xA1 , 0x28);

	/* ICW3 - setup cascading */
	ASM_write_port(0x21 , 0x00);
	ASM_write_port(0xA1 , 0x00);

	/* ICW4 - environment info */
	ASM_write_port(0x21 , 0x01);
	ASM_write_port(0xA1 , 0x01);
	/* Initialization finished */

	/* mask interrupts */
	ASM_write_port(0x21 , 0xff);
	ASM_write_port(0xA1 , 0xff);

	/* fill the IDT descriptor */
	iaddr = (int32_t)idt_table;
	iptr[0] = (sizeof (struct __IDT_ENTRY) * IDT_SIZE) + ((iaddr & 0xffff) << 16);
	iptr[1] = iaddr >> 16 ;

	ASM_load_idt(iptr);
}
